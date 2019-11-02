# V-JSONRPC
Basic JSON-RPC server written on V.

## Install
### VPM
```
v install nedpals.jsonrpc
```

### VPM
```
vpkg get v-jsonrpc
```

## Usage
```golang
module main

import jsonrpc // or 'import nedpals.jsonrpc' for vpm

// Function we will gonna use for "printName" procedure.
fn say_hello(req jsonrpc.Context) string {
	name := ctx.req.params['name']
	return 'Hello, $name'
}

fn main() {
    // Initialize and specify port
    jrpc := jsonrpc.new(8046)

    // Register procedures. Equivalent of "endpoints" if you are on REST/HTTP
    jrpc.register_procedure('printName', say_hello)

    // Start the server!
    jrpc.start_and_listen()
}
```

```bash
$ v run jrpc.v

```

### Error Handling
V-JSONRPC includes basic error handling as well as a set of public constants for easy use.
```golang
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
```

```golang
//... Function context must be mutable. e.g fn proc_name(ctx mut jsonrpc.Context)
    ctx.res.send_error(jsonrpc.JRPC_INVALID_REQUEST)
//...
```

```json
{
    "jsonrpc":"2.0",
    "id":0,
    "error":{
        "code":-32600,
        "message":"Invalid request.",
        "data":""
    },
    "result":""
}
```

## Limitations / Caveats
- Generic structs were not yet implemented. For now, it uses the `map[string]string{}` type for the params.
- Parsing `params` are not directly parsed to a map. Instead it goes to `RawRequest` first then converts the raw string into map and puts them into `Request`. May incur performance penalties.
- Responses may still have an `error` field regardless if the request was successful or not.
- Segmentation faults when triggering errors.
- This is still a **BASIC** implementation of the JSON-RPC server.

## Notes
Special huge thanks to [spytheman](https://github.com/spytheman/) for reviewing fixing the code!

## Contributing
1. Fork it (<https://github.com/nedpals/v-jsonrpc/fork>)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## License
[MIT](LICENSE)

## Contributors

- [Ned Palacios](https://github.com/nedpals) - creator and maintainer