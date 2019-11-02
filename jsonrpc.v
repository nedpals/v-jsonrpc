module main

import net
import json

const (
    JRPC_PARSE_ERROR = -32700
    JRPC_INVALID_REQUEST = -32600
    JRPC_METHOD_NOT_FOUND = -32601
    JRPC_INVALID_PARAMS = -32602
    JRPC_INTERNAL_ERROR = -32693    
    JRPC_SERVER_ERROR_START = -32099
    JRPC_SERVER_ERROR_END = -32600
    JRPC_SERVER_NOT_INITIALIZED = -32002
    JRPC_UNKNOWN_ERROR_CODE = -32001
    // JRPC_LSP_REQUEST_CANCELLED = -32800
    // JRPC_LSP_CONTENT_MODIFIED = -32801
    JRPC_VERSION = '2.0'
    JRPC_PORT = 8042
)

struct Header {
	name string
	value string
}

struct Procedure {
	name string
	func fn (Request) string
}

struct RawRequest {
    jsonrpc string
    id int
    method string
mut:
	// headers []Header [skip]
    params string [raw]
}

struct Request {
    jsonrpc string
    id int
    method string
mut:
    params map[string]string
}

struct Response {
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

struct Server {
	port int
mut:
	procs []Procedure
}

fn (res mut Response) send_error(err_code int) {
	mut error := ResponseError{ code: err_code, data: '' }

	error.message = match err_code {
		JRPC_PARSE_ERROR { 'Invalid JSON' }
		JRPC_INVALID_PARAMS { 'Invalid params.' }
		JRPC_INVALID_REQUEST { 'Invalid request.' }
		JRPC_METHOD_NOT_FOUND { 'Method not found.' }
		JRPC_SERVER_ERROR_END { 'Error while stopping the server.' }
		JRPC_SERVER_NOT_INITIALIZED { 'Server not yet initialized.' }
		JRPC_SERVER_ERROR_START { 'Error while starting the server.' }
		else { 'Unknown error.' }
	}

	res.error = error
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

fn (server Server) start() {
	listener := net.listen(server.port) or {panic("Failed to listen to port ${server.port}")}
	for {
		mut res := Response{ jsonrpc: JRPC_VERSION }
		conn := listener.accept() or {
			res.send_error(JRPC_SERVER_ERROR_START)
			return
		}
		s := conn.read_line()
		vals := s.split_into_lines()
		content := vals[vals.len-1]
		raw_req := json.decode(RawRequest, content) or {
			res.send_error(JRPC_INVALID_REQUEST)
			conn.close()
			return
		}
		req := process_request(raw_req) 

		if s == '' {
			res.send_error(JRPC_INTERNAL_ERROR)
			conn.close()
			return
		}

		if vals.len < 2 {
			res.send_error(JRPC_INVALID_REQUEST)
			conn.close()
			return
		}

		res.id = req.id
		proc_idx := server.proc_index(req.method)
		invoke_proc := server.procs[proc_idx].func
		res.result = invoke_proc(req)
		res.send(conn)
		conn.close()
	}
}

fn (server mut Server) register_procedure(method_name string, proc_func fn(Request, RawRequest) string) {
	proc := Procedure{ name: method_name, func: proc_func }
	server.procs << proc
}

fn new_jrpc(port_num int) Server {
	return Server{ port: port_num, procs: []Procedure }
}