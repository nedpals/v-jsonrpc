# V-JSONRPC
Basic JSON-RPC 2.0-compliant server written on V.

## Install
### VPM
```
v install nedpals.jsonrpc
```

### [vpkg](https://github.com/vpkg-project/vpkg)
```
vpkg get v-jsonrpc
```

## Usage
```golang
module main

import jsonrpc // or 'import nedpals.jsonrpc' for vpm

// Function we will gonna use for the "printName" procedure.
fn say_hello(ctx jsonrpc.Context) string {
	name := ctx.req.params['name']
	return 'Hello, $name'
}

fn main() {
    // Initialize
    jrpc := jsonrpc.new()

    // Register procedures. Equivalent of "endpoints" if you are on REST/HTTP
    jrpc.register_procedure('printName', say_hello)

    // Specify port and tart the server!
    jrpc.start_and_listen(8046)
}
```

```bash
$ v run jrpc.v
[I 2019-11-02 23:14:01] JSON-RPC Server has started on 8046
[E 2019-11-02 23:14:03] [ID: 1][dummy] Invalid request.
[I 2019-11-02 23:14:06] [ID: 0][printName] Procedure triggered.
```

### Error Handling
V-JSONRPC includes basic error handling as well as a set of public constants for easy use.
```golang
pub const (
    PARSE_ERROR = -32700
    INVALID_REQUEST = -32600
    METHOD_NOT_FOUND = -32601
    INVALID_PARAMS = -32602
    INTERNAL_ERROR = -32693    
    SERVER_ERROR_START = -32099
    SERVER_ERROR_END = -32600
    SERVER_NOT_INITIALIZED = -32002
    UNKNOWN_ERROR_CODE = -32001
)
```

```golang
//... Function context must be mutable. e.g fn proc_name(ctx mut jsonrpc.Context)
    ctx.res.send_error(jsonrpc.INVALID_REQUEST)
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
    }
}
```

## Limitations / Caveats
- Generic structs were not implemented yet. For now, it uses the `map[string]string{}` type for the params.
- Parsing `params` are not directly parsed to a map. Instead it goes to `RawRequest` first then converts the raw string into a map and puts them into `Request`. May incur performance penalties.
- ~~ Responses may still have an `error` field regardless if the request was successful or not.~~
- Segmentation faults when triggering errors.
- This is still a **BASIC** implementation of the JSON-RPC server.

## Notes
Special huge thanks to [spytheman](https://github.com/spytheman/) for reviewing and fixing the code!

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
