module main

fn print_name(req Request) string {
	name := req.params['name']
	return 'Hello, $name'
}

fn main() {
	mut jrpc := new_jrpc(JRPC_PORT)
	jrpc.register_procedure('printName', print_name)

	jrpc.start()
}