module jsonrpc

import (
	net
	json
	log
)

pub const (
    PARSE_ERROR = -32700
    INVALID_REQUEST = -32600
    METHOD_NOT_FOUND = -32601
    INVALID_PARAMS = -32602
    INTERNAL_ERROR = -32693    
    SERVER_ERROR_START = -32099
    SERVER_ERROR_END = -32600
    SERVER_NOT_INITIALIZED = -32002
    UNKNOWN_ERROR = -32001
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
		PARSE_ERROR { 'Invalid JSON' }
		INVALID_PARAMS { 'Invalid params.' }
		INVALID_REQUEST { 'Invalid request.' }
		METHOD_NOT_FOUND { 'Method not found.' }
		SERVER_ERROR_END { 'Error while stopping the server.' }
		SERVER_NOT_INITIALIZED { 'Server not yet initialized.' }
		SERVER_ERROR_START { 'Error while starting the server.' }
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

fn process_raw_request(json_str string, raw_contents string) RawRequest {
	mut raw_req := RawRequest{}
	raw_req.headers = http.parse_headers(raw_contents.split_into_lines())

	if json_str == '{}' {
		return raw_req
	} else {
		from_json := json.decode(RawRequest, json_str) or { return raw_req }
		raw_req.jsonrpc = from_json.jsonrpc
		raw_req.id = from_json.id
		raw_req.method = from_json.method
		raw_req.params = from_json.params
	}

	return raw_req
}
	listener := net.listen(server.port) or {panic('Failed to listen to port ${server.port}')}
	mut log := log.Log{ level: 4, output: 'terminal' }

	log.info('JSON-RPC Server has started on port ${server.port}')
	for {
		mut res := Response{ jsonrpc: JRPC_VERSION }
		conn := listener.accept() or {
			logg.set_level(1)
			logg.error(err_message(SERVER_ERROR_START))
			res.send_error(SERVER_ERROR_START)
			return
		}
		s := conn.read_line()
		vals := s.split_into_lines()
		content := vals[vals.len-1]
		raw_req := process_raw_request(content, s)
		req := process_request(raw_req) 

		if s == '' {
			logg.set_level(2)
			logg.error(err_message(INTERNAL_ERROR))
			res.send_error(INTERNAL_ERROR)
		}

		if content == '{}' || content == '' || vals.len < 2 {
		}

		res.id = req.id
		proc_idx := server.proc_index(req.method)

		ctx := Context{res: res, req: req, raw: raw_req}

		if proc_idx != -1 {
			invoke_proc := server.procs[proc_idx].func
			proc_name := server.procs[proc_idx].name
			res.result = invoke_proc(ctx)
			logg.set_level(4)
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
