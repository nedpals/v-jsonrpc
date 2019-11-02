module jsonrpc

import net
import json
import log

pub const (
    JRPC_PARSE_ERROR = -32700
    JRPC_INVALID_REQUEST = -32600
    JRPC_METHOD_NOT_FOUND = -32601
    JRPC_INVALID_PARAMS = -32602
    JRPC_INTERNAL_ERROR = -32693    
    JRPC_SERVER_ERROR_START = -32099
    JRPC_SERVER_ERROR_END = -32600
    JRPC_SERVER_NOT_INITIALIZED = -32002
    JRPC_UNKNOWN_ERROR_CODE = -32001
)

const (
    JRPC_VERSION = '2.0'
)

pub struct Context {
pub mut:
	res Response
	req Request
	raw RawRequest
}

struct Header {
	name string
	value string
}

struct Procedure {
	name string
	func fn (Context) string
}

struct RawRequest {
    jsonrpc string
    id int
    method string
mut:
	headers []Header [skip]
    params string [raw]
}

pub struct Request {
pub:
    jsonrpc string
    id int
    method string
mut:
    params map[string]string
}

pub struct Response {
    jsonrpc string
mut:
    id int
    error ResponseError [json:error]
    result string
}

struct ResponseError {
mut:
    code int
    message string
    data string
}

pub struct Server {
pub:
	port int
mut:
	procs []Procedure
}

pub fn (res mut Response) send_error(err_code int) {
	mut error := ResponseError{ code: err_code, data: '' }
	error.message = err_message(err_code)
	res.error = error
}

fn err_message(err_code int) string {
	msg := match err_code {
		JRPC_PARSE_ERROR { 'Invalid JSON' }
		JRPC_INVALID_PARAMS { 'Invalid params.' }
		JRPC_INVALID_REQUEST { 'Invalid request.' }
		JRPC_METHOD_NOT_FOUND { 'Method not found.' }
		JRPC_SERVER_ERROR_END { 'Error while stopping the server.' }
		JRPC_SERVER_NOT_INITIALIZED { 'Server not yet initialized.' }
		JRPC_SERVER_ERROR_START { 'Error while starting the server.' }
		else { 'Unknown error.' }
	}

	return msg
}

fn (res Response) json() string {
	return json.encode(res)
}

fn (err ResponseError) str() string {
	return json.encode(err)
}

fn (res &Response) send(conn net.Socket) {
	res_json := res.json()

	conn.write('Content-Length: ${res_json.len}\r')
	conn.write('')
	conn.write(res_json)
}

fn process_request(raw_req RawRequest) Request {
	mut req := Request{JRPC_VERSION, raw_req.id, raw_req.method, map[string]string}
	params_arr := raw_req.params.find_between('{', '}').split(',')

	for pkv in params_arr {
		p := pkv.split(':')
		key := p[0].find_between('"', '"')
		val := p[1].find_between('"', '"')

		req.params[key] = val
	}

	return req
}

fn (server Server) proc_index(name string) int {
	for i, proc in server.procs {
		if proc.name == name {
			return i
		}
	}

	return -1
}

pub fn (server Server) start_and_listen() {
	listener := net.listen(server.port) or {panic('Failed to listen to port ${server.port}')}
	mut log := log.Log{ level: 4, output: 'terminal' }

	log.info('JSON-RPC Server has started on port ${server.port}')
	for {
		mut res := Response{ jsonrpc: JRPC_VERSION }
		conn := listener.accept() or {
			log.set_level(1)
			log.error(err_message(JRPC_SERVER_ERROR_START))
			res.send_error(JRPC_SERVER_ERROR_START)
			return
		}
		s := conn.read_line()
		vals := s.split_into_lines()
		content := vals[vals.len-1]
		raw_req := json.decode(RawRequest, content) or {
			log.set_level(2)
			log.error(err_message(JRPC_INVALID_REQUEST))
			res.send_error(JRPC_INVALID_REQUEST)
			conn.close()
			return
		}
		req := process_request(raw_req) 

		if s == '' {
			log.set_level(2)
			log.error(err_message(JRPC_INTERNAL_ERROR))
			res.send_error(JRPC_INTERNAL_ERROR)
			conn.close()
			return
		}

		if vals.len < 2 {
			log.set_level(2)
			log.error(err_message(JRPC_INVALID_REQUEST))
			res.send_error(JRPC_INVALID_REQUEST)
			conn.close()
			return
		}

		res.id = req.id
		proc_idx := server.proc_index(req.method)

		ctx := Context{res: res, req: req, raw: raw_req}

		if proc_idx != -1 {
			invoke_proc := server.procs[proc_idx].func
			proc_name := server.procs[proc_idx].name
			res.result = invoke_proc(ctx)
			log.set_level(4)
			log.info('[ID: ${req.id}][${req.method}] Procedure triggered.')
		} else {
			log.set_level(2)
			log.error('[ID: ${req.id}][${req.method}] ' + err_message(JRPC_INVALID_REQUEST))
			res.send_error(JRPC_INVALID_REQUEST)
		}

		res.send(conn)
		conn.close()
	}
}

pub fn (server mut Server) register_procedure(method_name string, proc_func fn (Context) string) {
	proc := Procedure{ name: method_name, func: proc_func }
	server.procs << proc
}

pub fn new(port_num int) Server {
	return Server{ port: port_num, procs: []Procedure }
}
