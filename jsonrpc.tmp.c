 #ifndef V_COMMIT_HASH
 #define V_COMMIT_HASH "7b1993b"
 #endif
 

#include <stdio.h>  // TODO remove all these includes, define all function signatures and types manually
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h> // for va_list
#include <inttypes.h>  // int64_t etc
#include <string.h> // memcpy

#ifndef _WIN32
#include <ctype.h>
#include <locale.h> // tolower
#include <sys/time.h>
#include <unistd.h> // sleep	
#endif


#ifdef __linux__
#include <sys/types.h>
#include <sys/wait.h> // os__wait uses wait on nix
#endif

#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/wait.h> // os__wait uses wait on nix
#endif

#ifdef __DragonFly__
#include <sys/types.h>
#include <sys/wait.h> // os__wait uses wait on nix
#endif

#ifdef __OpenBSD__
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h> // os__wait uses wait on nix
#endif

#define EMPTY_STRUCT_DECLARATION
#define EMPTY_STRUCT_INITIALIZATION 0
// Due to a tcc bug, the length of an array needs to be specified, but GCC crashes if it is...
#define EMPTY_ARRAY_OF_ELEMS(x,n) (x[])
#define TCCSKIP(x) x

#ifdef __TINYC__
#undef EMPTY_STRUCT_INITIALIZATION
#define EMPTY_STRUCT_INITIALIZATION
#undef EMPTY_ARRAY_OF_ELEMS
#define EMPTY_ARRAY_OF_ELEMS(x,n) (x[n])
#undef TCCSKIP
#define TCCSKIP(x)
#endif

#define OPTION_CAST(x) (x)

#ifdef _WIN32
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _UNICODE
#define UNICODE
#include <windows.h>

// must be included after <windows.h>
#ifndef __TINYC__
#include <shellapi.h>
#endif

#include <io.h> // _waccess
#include <fcntl.h> // _O_U8TEXT
#include <direct.h> // _wgetcwd
//#include <WinSock2.h>
#ifdef _MSC_VER
// On MSVC these are the same (as long as /volatile:ms is passed)
#define _Atomic volatile

// MSVC cannot parse some things properly
#undef EMPTY_STRUCT_DECLARATION
#undef OPTION_CAST

#define EMPTY_STRUCT_DECLARATION int ____dummy_variable
#define OPTION_CAST(x)
#endif

#else
#include <pthread.h>
#endif

//================================== TYPEDEFS ================================*/

typedef int64_t i64;
typedef int16_t i16;
typedef int8_t i8;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t byte;
typedef uint32_t rune;
typedef float f32;
typedef double f64;
typedef unsigned char* byteptr;
typedef int* intptr;
typedef void* voidptr;
typedef struct array array;
typedef struct map map;
typedef array array_string;
typedef array array_int;
typedef array array_byte;
typedef array array_f32;
typedef array array_f64;
typedef map map_int;
typedef map map_string;
#ifndef bool
	typedef int bool;
	#define true 1
	#define false 0
#endif

//============================== HELPER C MACROS =============================*/
#define _PUSH(arr, val, tmp, tmp_typ) {tmp_typ tmp = (val); array_push(arr, &tmp);}
#define _PUSH_MANY(arr, val, tmp, tmp_typ) {tmp_typ tmp = (val); array_push_many(arr, tmp.data, tmp.len);}
#define _IN(typ, val, arr) array_##typ##_contains(arr, val)
#define _IN_MAP(val, m) map_exists(m, val)
#define DEFAULT_EQUAL(a, b) (a == b)
#define DEFAULT_NOT_EQUAL(a, b) (a != b)
#define DEFAULT_LT(a, b) (a < b)
#define DEFAULT_LE(a, b) (a <= b)
#define DEFAULT_GT(a, b) (a > b)
#define DEFAULT_GE(a, b) (a >= b)
//================================== GLOBALS =================================*/
byteptr g_str_buf;
int load_so(byteptr);
void reload_so();

 #include "cJSON.h"
 int g_test_oks = 0;
 int g_test_fails = 0;
 
#define js_get(object, key) cJSON_GetObjectItemCaseSensitive((object), (key))

 // this line will be replaced with definitions
 array new_array(int mylen, int cap, int elm_size) {
 
array arr= (array) { .len =  mylen , .cap =  cap , .element_size =  elm_size , .data =  v_calloc ( cap * elm_size ) } ;
 
return  arr ;
 
 }
 array make(int len, int cap, int elm_size) {
 
return  new_array ( len , cap , elm_size ) ;
 
 }
 array new_array_from_c_array(int len, int cap, int elm_size, void* c_array) {
 
array arr= (array) { .len =  len , .cap =  cap , .element_size =  elm_size , .data =  v_calloc ( cap * elm_size ) } ;
 
 memcpy ( arr .data ,  c_array ,  len * elm_size ) ;
 
return  arr ;
 
 }
 array new_array_from_c_array_no_alloc(int len, int cap, int elm_size, void* c_array) {
 
array arr= (array) { .len =  len , .cap =  cap , .element_size =  elm_size , .data =  c_array } ;
 
return  arr ;
 
 }
 void array_ensure_cap(array* a, int required) {
 
 if ( required > a ->cap ) {
 
int cap= ( a ->cap == 0 ) ? ( 2 ) : ( a ->cap * 2 ) ;
 
 while ( required > cap ) {
 
 
 cap  *=  2 ;
 
 }
 ;
 
 if ( a ->cap == 0 ) {
 
 a ->data  =  v_calloc ( cap * a ->element_size ) ;
 
 }
  else { 
 
 a ->data  =  realloc ( a ->data ,  cap * a ->element_size ) ;
 
 }
 ;
 
 a ->cap  =  cap ;
 
 }
 ;
 
 }
 array array_repeat_old(void* val, int nr_repeats, int elm_size) {
 
 if ( nr_repeats < 0 ) {
 
 v_panic ( _STR("[0; len]: `len` is negative (len == %d)", nr_repeats) ) ;
 
 }
 ;
 
array arr= (array) { .len =  nr_repeats , .cap =  nr_repeats , .element_size =  elm_size , .data =  v_calloc ( nr_repeats * elm_size ) } ;
 
 for (
int i= 0  ;  i < nr_repeats  ;  i ++ ) { 
 
 
 memcpy ((byte*) arr .data + i * elm_size ,  val ,  elm_size ) ;
 
 }
 ;
 
return  arr ;
 
 }
 array array_repeat(array a, int nr_repeats) {
 
 if ( nr_repeats < 0 ) {
 
 v_panic ( _STR("array.repeat: count is negative (count == %d)", nr_repeats) ) ;
 
 }
 ;
 
array arr= (array) { .len =  nr_repeats * a .len , .cap =  nr_repeats * a .len , .element_size =  a .element_size , .data =  v_calloc ( nr_repeats * a .len * a .element_size ) } ;
 
 for (
int i= 0  ;  i < nr_repeats  ;  i ++ ) { 
 
 
 memcpy ((byte*) arr .data + i * a .len * a .element_size ,  a .data ,  a .len * a .element_size ) ;
 
 }
 ;
 
return  arr ;
 
 }
 void array_sort_with_compare(array* a, void* compare) {
 
 qsort ( a ->data ,  a ->len ,  a ->element_size ,  compare ) ;
 
 }
 void array_insert(array* a, int i, void* val) {
 
 if ( i < 0  ||  i > a ->len ) {
 
 v_panic ( _STR("array.insert: index out of range (i == %d, a.len == %d)", i, a ->len) ) ;
 
 }
 ;
 
 array_ensure_cap ( a , a ->len + 1 ) ;
 
int size= a ->element_size ;
 
 memmove ((byte*) a ->data + ( i + 1 ) * size , (byte*) a ->data + i * size ,  ( a ->len - i ) * size ) ;
 
 memcpy ((byte*) a ->data + i * size ,  val ,  size ) ;
 
 a ->len ++ ;
 
 }
 void array_prepend(array* a, void* val) {
 
 array_insert ( a , 0 , val ) ;
 
 }
 void v_array_delete(array* a, int i) {
 
 if ( i < 0  ||  i >= a ->len ) {
 
 v_panic ( _STR("array.delete: index out of range (i == %d, a.len == %d)", i, a ->len) ) ;
 
 }
 ;
 
int size= a ->element_size ;
 
 memmove ((byte*) a ->data + i * size , (byte*) a ->data + ( i + 1 ) * size ,  ( a ->len - i ) * size ) ;
 
 a ->len -- ;
 
 }
 void* array_get(array a, int i) {
 
 if ( i < 0  ||  i >= a .len ) {
 
 v_panic ( _STR("array.get: index out of range (i == %d, a.len == %d)", i, a .len) ) ;
 
 }
 ;
 
return (byte*) a .data + i * a .element_size ;
 
 }
 void* array_first(array a) {
 
 if ( a .len == 0 ) {
 
 v_panic ( tos3("array.first: array is empty") ) ;
 
 }
 ;
 
return (byte*) a .data + 0 ;
 
 }
 void* array_last(array a) {
 
 if ( a .len == 0 ) {
 
 v_panic ( tos3("array.last: array is empty") ) ;
 
 }
 ;
 
return (byte*) a .data + ( a .len - 1 ) * a .element_size ;
 
 }
 array array_left(array a, int n) {
 
 if ( n < 0 ) {
 
 v_panic ( _STR("array.left: index is negative (n == %d)", n) ) ;
 
 }
 ;
 
 if ( n >= a .len ) {
 
return  array_slice ( a , 0 , a .len ) ;
 
 }
 ;
 
return  array_slice ( a , 0 , n ) ;
 
 }
 array array_right(array a, int n) {
 
 if ( n < 0 ) {
 
 v_panic ( _STR("array.right: index is negative (n == %d)", n) ) ;
 
 }
 ;
 
 if ( n >= a .len ) {
 
return  new_array ( 0 , 0 , a .element_size ) ;
 
 }
 ;
 
return  array_slice ( a , n , a .len ) ;
 
 }
 array array_slice2(array a, int start, int _end, bool end_max) {
 
int end= ( end_max ) ? ( a .len ) : ( _end ) ;
 
return  array_slice ( a , start , end ) ;
 
 }
 array array_slice(array a, int start, int _end) {
 
int end= _end ;
 
 if ( start > end ) {
 
 v_panic ( _STR("array.slice: invalid slice index (%d > %d)", start, end) ) ;
 
 }
 ;
 
 if ( end > a .len ) {
 
 v_panic ( _STR("array.slice: slice bounds out of range (%d >= %d)", end, a .len) ) ;
 
 }
 ;
 
 if ( start < 0 ) {
 
 v_panic ( _STR("array.slice: slice bounds out of range (%d < 0)", start) ) ;
 
 }
 ;
 
int l= end - start ;
 
array res= (array) { .element_size =  a .element_size , .data = (byte*) a .data + start * a .element_size , .len =  l , .cap =  l } ;
 
return  res ;
 
 }
 void array_set(array* a, int i, void* val) {
 
 if ( i < 0  ||  i >= a ->len ) {
 
 v_panic ( _STR("array.set: index out of range (i == %d, a.len == %d)", i, a ->len) ) ;
 
 }
 ;
 
 memcpy ((byte*) a ->data + a ->element_size * i ,  val ,  a ->element_size ) ;
 
 }
 void array_push(array* a, void* val) {
 
 array_ensure_cap ( a , a ->len + 1 ) ;
 
 memcpy ((byte*) a ->data + a ->element_size * a ->len ,  val ,  a ->element_size ) ;
 
 a ->len ++ ;
 
 }
 void array_push_many(array* a, void* val, int size) {
 
 array_ensure_cap ( a , a ->len + size ) ;
 
 memcpy ((byte*) a ->data + a ->element_size * a ->len ,  val ,  a ->element_size * size ) ;
 
 a ->len  +=  size ;
 
 }
 array array_reverse(array a) {
 
array arr= (array) { .len =  a .len , .cap =  a .cap , .element_size =  a .element_size , .data =  v_calloc ( a .cap * a .element_size ) } ;
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
 memcpy ((byte*) arr .data + i * arr .element_size ,  & ( *(array*) array_get(  a , a .len - 1 - i) ) ,  arr .element_size ) ;
 
 }
 ;
 
return  arr ;
 
 }
 array array_clone(array a) {
 
array arr= (array) { .len =  a .len , .cap =  a .cap , .element_size =  a .element_size , .data =  v_calloc ( a .cap * a .element_size ) } ;
 
 memcpy ( arr .data ,  a .data ,  a .cap * a .element_size ) ;
 
return  arr ;
 
 }
 void v_array_free(array a) {
 
 free ( a .data ) ;
 
 }
 string array_string_str(array_string a) {
 
strings__Builder sb= strings__new_builder ( a .len * 3 ) ;
 
 strings__Builder_write (& /* ? */ sb , tos3("[") ) ;
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
string val= ( *(string*) array_get(  a , i) ) ;
 
 strings__Builder_write (& /* ? */ sb , tos3("\"") ) ;
 
 strings__Builder_write (& /* ? */ sb , val ) ;
 
 strings__Builder_write (& /* ? */ sb , tos3("\"") ) ;
 
 if ( i < a .len - 1 ) {
 
 strings__Builder_write (& /* ? */ sb , tos3(", ") ) ;
 
 }
 ;
 
 }
 ;
 
 strings__Builder_write (& /* ? */ sb , tos3("]") ) ;
 
return  strings__Builder_str ( sb ) ;
 
 }
 string array_bool_str(array_bool a) {
 
strings__Builder sb= strings__new_builder ( a .len * 3 ) ;
 
 strings__Builder_write (& /* ? */ sb , tos3("[") ) ;
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
bool val= ( *(bool*) array_get(  a , i) ) ;
 
 if ( val ) {
 
 strings__Builder_write (& /* ? */ sb , tos3("true") ) ;
 
 }
  else { 
 
 strings__Builder_write (& /* ? */ sb , tos3("false") ) ;
 
 }
 ;
 
 if ( i < a .len - 1 ) {
 
 strings__Builder_write (& /* ? */ sb , tos3(", ") ) ;
 
 }
 ;
 
 }
 ;
 
 strings__Builder_write (& /* ? */ sb , tos3("]") ) ;
 
return  strings__Builder_str ( sb ) ;
 
 }
 string array_byte_hex(array_byte b) {
 
byte* hex= v_malloc ( b .len * 2 + 1 ) ;
 
byte* ptr= & hex [/*ptr*/ 0 ]/*rbyte 1*/ ;
 
 for (
int i= 0  ;  i < b .len  ;  i ++ ) { 
 
 
 ptr  +=  sprintf ( ((char*)( ptr ) ) ,  "%02x" ,  ( *(byte*) array_get(  b , i) ) ) ;
 
 }
 ;
 
return  (tos2((byte *) hex ) ) ;
 
 }
 int copy(array_byte dst, array_byte src) {
 
 if ( dst .len > 0  &&  src .len > 0 ) {
 
int min= ( dst .len < src .len ) ? ( dst .len ) : ( src .len ) ;
 
 memcpy ( dst .data ,  array_left ( src , min ) .data ,  dst .element_size * min ) ;
 
return  min ;
 
 }
 ;
 
return  0 ;
 
 }
 int compare_ints(int a, int b) {
 
 if ( a < b ) {
 
return  - 1 ;
 
 }
 ;
 
 if ( a > b ) {
 
return  1 ;
 
 }
 ;
 
return  0 ;
 
 }
 void array_int_sort(array_int* a) {
 
 array_sort_with_compare ( a ,& /*112 EXP:"void*" GOT:"fn (int,int) int" */ compare_ints ) ;
 
 }
 int array_string_index(array_string a, string v) {
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
 if (string_eq( ( *(string*) array_get(  a , i) ) , v ) ) {
 
return  i ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int array_int_index(array_int a, int v) {
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
 if ( ( *(int*) array_get(  a , i) ) == v ) {
 
return  i ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int array_byte_index(array_byte a, byte v) {
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
 if ( ( *(byte*) array_get(  a , i) ) == v ) {
 
return  i ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int array_char_index(array_char a, char v) {
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
 if ( ( *(char*) array_get(  a , i) ) == v ) {
 
return  i ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int array_int_reduce(array_int a, int (*iter)( int accum, int curr /*FFF*/ ), int accum_start) {
 
int _accum= 0 ;
 
 _accum  =  accum_start ;
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
 _accum  =  iter ( _accum , ( *(int*) array_get(  a , i) ) ) ;
 
 }
 ;
 
return  _accum ;
 
 }
 bool array_int_eq(array_int a, array_int a2) {
 
return  array_eq_T_int ( a , a2 ) ;
 
 }
 bool array_i64_eq(array_i64 a, array_i64 a2) {
 
return  array_eq_T_i64 ( a , a2 ) ;
 
 }
 bool array_string_eq(array_string a, array_string a2) {
 
return  array_eq_T_string ( a , a2 ) ;
 
 }
 bool array_byte_eq(array_byte a, array_byte a2) {
 
return  array_eq_T_byte ( a , a2 ) ;
 
 }
 bool array_f32_eq(array_f32 a, array_f32 a2) {
 
return  array_eq_T_f32 ( a , a2 ) ;
 
 }
 void builtin__init() {
 
 #ifdef _WIN32
 
 if ( is_atty ( 0 ) ) {
 
 _setmode ( _fileno ( stdin ) ,  _O_U16TEXT ) ;
 
 }
  else { 
 
 _setmode ( _fileno ( stdin ) ,  _O_U8TEXT ) ;
 
 }
 ;
 
 _setmode ( _fileno ( stdout ) ,  _O_U8TEXT ) ;
 
 SetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ) ,  ENABLE_PROCESSED_OUTPUT | 0x0004 ) ;
 
 setbuf ( stdout ,  0 ) ;
 
 #endif
 ;
 
 }
 void v_exit(int code) {
 
 exit ( code ) ;
 
 }
 bool isnil(void* v) {
 
return  v == 0 ;
 
 }
 void on_panic(int (*f)( int  /*FFF*/ )) {
 
 }
 void print_backtrace_skipping_top_frames(int skipframes) {
 
 #ifdef __APPLE__
 
byte* buffer  [100 ] ;
 
int nr_ptrs= backtrace ( ((voidptr*)( buffer ) ) ,  100 ) ;
 
 backtrace_symbols_fd ( ((voidptr*)( & buffer [ skipframes ]/*rbyte* 0*/ ) ) ,  nr_ptrs - skipframes ,  1 ) ;
 
 
 return ;
 
 #endif
 ;
 
 #ifdef __linux__
 
 #ifndef __BIONIC__
 
 #ifdef __GLIBC__
 
byte* buffer  [100 ] ;
 
int nr_ptrs= backtrace ( ((voidptr*)( buffer ) ) ,  100 ) ;
 
int nr_actual_frames= nr_ptrs - skipframes ;
 
array_string sframes=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
byteptr* csymbols= ((byteptr*)( backtrace_symbols ( ((voidptr*)( & buffer [ skipframes ]/*rbyte* 0*/ ) ) ,  nr_actual_frames ) ) ) ;
 
 int tmp1 =  0;
 ;
for (int tmp2 = tmp1; tmp2 <  nr_actual_frames; tmp2++) {
 int i = tmp2;
 
 
_PUSH(& sframes , ( /*typ = array_string   tmp_typ=string*/ tos2 ( csymbols [/*ptr*/ i ]/*rbyteptr 0*/ ) ), tmp3, string) ;
 
 }
 ;
 
 array_string tmp4 =  sframes;
 for (int tmp5 = 0; tmp5 < tmp4.len; tmp5++) {
 string sframe = ((string *) tmp4 . data)[tmp5];
 
 
string executable= string_all_before ( sframe , tos3("(") ) ;
 
string addr= string_all_before ( string_all_after ( sframe , tos3("[") ) , tos3("]") ) ;
 
string cmd= _STR("addr2line -e %.*s %.*s", executable.len, executable.str, addr.len, addr.str) ;
 
void* f= popen ((char*) cmd .str ,  "r" ) ;
 
 if ( isnil ( f ) ) {
 
 println ( sframe ) ;
 
 continue
 ;
 
 }
 ;
 
byte buf  [1000 ] = { 0 } ;
 
string output= tos3("") ;
 
 while ( fgets ( buf ,  1000 ,  f ) != 0 ) {
 
 
 output = string_add(output,  tos ( buf , vstrlen ( buf ) ) ) ;
 
 }
 ;
 
 output  = string_add( string_trim_space ( output ) , tos3(":") ) ;
 
 if ( 0 != ((int)( pclose ( f ) ) ) ) {
 
 println ( sframe ) ;
 
 continue
 ;
 
 }
 ;
 
printf( "%-45s | %.*s\n", output.str, sframe.len, sframe.str ) ;
 
 }
 ;
 
 
 return ;
 
 #else
 
 printf ( "backtrace_symbols_fd is missing, so printing backtraces is not available.\n" ) ;
 
 printf ( "Some libc implementations like musl simply do not provide it.\n" ) ;
 
 #endif
 ;
 
 #endif
 ;
 
 #endif
 ;
 
 println ( tos3("print_backtrace_skipping_top_frames is not implemented on this platform for now...\n") ) ;
 
 }
 void print_backtrace() {
 
 print_backtrace_skipping_top_frames ( 2 ) ;
 
 }
 void panic_debug(int line_no, string file, string mod, string fn_name, string s) {
 
 println ( tos3("================ V panic ================") ) ;
 
printf( "   module: %.*s\n", mod.len, mod.str ) ;
 
printf( " function: %.*s()\n", fn_name.len, fn_name.str ) ;
 
printf( "     file: %.*s\n", file.len, file.str ) ;
 
 println (string_add( tos3("     line: ") , int_str ( line_no ) ) ) ;
 
printf( "  message: %.*s\n", s.len, s.str ) ;
 
 println ( tos3("=========================================") ) ;
 
 print_backtrace_skipping_top_frames ( 1 ) ;
 
 exit ( 1 ) ;
 
 }
 void v_panic(string s) {
 
printf( "V panic: %.*s\n", s.len, s.str ) ;
 
 print_backtrace ( ) ;
 
 exit ( 1 ) ;
 
 }
 void println(string s) {
 
 if ( isnil ( s .str ) ) {
 
 v_panic ( tos3("println(NIL)") ) ;
 
 }
 ;
 
 #ifdef _WIN32
 
 _putws ( string_to_wide ( s ) ) ;
 
 #else
 
 printf ( "%.*s\n" ,  s .len , (char*) s .str ) ;
 
 #endif
 ;
 
 }
 void eprintln(string s) {
 
 if ( isnil ( s .str ) ) {
 
 v_panic ( tos3("eprintln(NIL)") ) ;
 
 }
 ;
 
 #ifdef __APPLE__
 
 fprintf ( stderr ,  "%.*s\n" ,  s .len , (char*) s .str ) ;
 
 fflush ( stderr ) ;
 
 
 return ;
 
 #endif
 ;
 
 #ifdef __linux__
 
 fprintf ( stderr ,  "%.*s\n" ,  s .len , (char*) s .str ) ;
 
 fflush ( stderr ) ;
 
 
 return ;
 
 #endif
 ;
 
 println ( s ) ;
 
 }
 void print(string s) {
 
 #ifdef _WIN32
 
 wprintf ( string_to_wide ( s ) ) ;
 
 #else
 
 printf ( "%.*s" ,  s .len , (char*) s .str ) ;
 
 #endif
 ;
 
 }
 byte* v_malloc(int n) {
 
 if ( n < 0 ) {
 
 v_panic ( tos3("malloc(<0)") ) ;
 
 }
 ;
 
byte* ptr= malloc ( n ) ;
 
 if ( isnil ( ptr ) ) {
 
 v_panic ( _STR("malloc(%d) failed", n) ) ;
 
 }
 ;
 
return  ptr ;
 
 }
 byte* v_calloc(int n) {
 
 if ( n < 0 ) {
 
 v_panic ( tos3("calloc(<0)") ) ;
 
 }
 ;
 
return  calloc ( n ,  1 ) ;
 
 }
 void v_free(void* ptr) {
 
 free ( ptr ) ;
 
 }
 void* memdup(void* src, int sz) {
 
byte* mem= v_malloc ( sz ) ;
 
return  memcpy ((char*) mem ,  src ,  sz ) ;
 
 }
 void v_ptr_free(void* ptr) {
 
 free ( ptr ) ;
 
 }
 bool is_atty(int fd) {
 
 #ifdef _WIN32
 
int mode= 0 ;
 
 GetConsoleMode ( _get_osfhandle ( fd ) ,  & mode ) ;
 
return  mode > 0 ;
 
 #else
 
return  isatty ( fd ) != 0 ;
 
 #endif
 ;
 
 }
 int backtrace(void* a, int b);
 byteptr* backtrace_symbols(void* , int );
 void backtrace_symbols_fd(void* , int , int );
 int proc_pidpath(int , void* , int );
 string f64_str(f64 d) {
 
byte* buf= v_malloc ( sizeof( double) * 5 + 1 ) ;
 
 sprintf ( ((char*)( buf ) ) ,  "%f" ,  d ) ;
 
return  tos ( buf , vstrlen ( buf ) ) ;
 
 }
 string f32_str(f32 d) {
 
byte* buf= v_malloc ( sizeof( double) * 5 + 1 ) ;
 
 sprintf ( ((char*)( buf ) ) ,  "%f" ,  d ) ;
 
return  tos ( buf , vstrlen ( buf ) ) ;
 
 }
 f32 f32_abs(f32 a) {
 
return  ( a < 0 ) ? ( - a ) : ( a ) ;
 
 }
 f64 f64_abs(f64 a) {
 
return  ( a < 0 ) ? ( - a ) : ( a ) ;
 
 }
 bool f64_eq(f64 a, f64 b) {
 
return  f64_abs ( a - b ) <= DBL_EPSILON ;
 
 }
 bool f32_eq(f32 a, f32 b) {
 
return  f32_abs ( a - b ) <= FLT_EPSILON ;
 
 }
 bool f64_eqbit(f64 a, f64 b) {
 
return  DEFAULT_EQUAL ( a ,  b ) ;
 
 }
 bool f32_eqbit(f32 a, f32 b) {
 
return  DEFAULT_EQUAL ( a ,  b ) ;
 
 }
 bool f64_ne(f64 a, f64 b) {
 
return  ! f64_eq ( a , b ) ;
 
 }
 bool f32_ne(f32 a, f32 b) {
 
return  ! f32_eq ( a , b ) ;
 
 }
 bool f64_nebit(f64 a, f64 b) {
 
return  DEFAULT_NOT_EQUAL ( a ,  b ) ;
 
 }
 bool f32_nebit(f32 a, f32 b) {
 
return  DEFAULT_NOT_EQUAL ( a ,  b ) ;
 
 }
 bool f64_lt(f64 a, f64 b) {
 
return  f64_ne ( a , b )  &&  f64_ltbit ( a , b ) ;
 
 }
 bool f32_lt(f32 a, f32 b) {
 
return  f32_ne ( a , b )  &&  f32_ltbit ( a , b ) ;
 
 }
 bool f64_ltbit(f64 a, f64 b) {
 
return  DEFAULT_LT ( a ,  b ) ;
 
 }
 bool f32_ltbit(f32 a, f32 b) {
 
return  DEFAULT_LT ( a ,  b ) ;
 
 }
 bool f64_le(f64 a, f64 b) {
 
return  ! f64_gt ( a , b ) ;
 
 }
 bool f32_le(f32 a, f32 b) {
 
return  ! f32_gt ( a , b ) ;
 
 }
 bool f64_lebit(f64 a, f64 b) {
 
return  DEFAULT_LE ( a ,  b ) ;
 
 }
 bool f32_lebit(f32 a, f32 b) {
 
return  DEFAULT_LE ( a ,  b ) ;
 
 }
 bool f64_gt(f64 a, f64 b) {
 
return  f64_ne ( a , b )  &&  f64_gtbit ( a , b ) ;
 
 }
 bool f32_gt(f32 a, f32 b) {
 
return  f32_ne ( a , b )  &&  f32_gtbit ( a , b ) ;
 
 }
 bool f64_gtbit(f64 a, f64 b) {
 
return  DEFAULT_GT ( a ,  b ) ;
 
 }
 bool f32_gtbit(f32 a, f32 b) {
 
return  DEFAULT_GT ( a ,  b ) ;
 
 }
 bool f64_ge(f64 a, f64 b) {
 
return  ! f64_lt ( a , b ) ;
 
 }
 bool f32_ge(f32 a, f32 b) {
 
return  ! f32_lt ( a , b ) ;
 
 }
 bool f64_gebit(f64 a, f64 b) {
 
return  DEFAULT_GE ( a ,  b ) ;
 
 }
 bool f32_gebit(f32 a, f32 b) {
 
return  DEFAULT_GE ( a ,  b ) ;
 
 }
 hashmap new_hashmap(int planned_nr_items) {
 
int cap= planned_nr_items * 5 ;
 
 if ( cap < builtin__min_cap ) {
 
 cap  =  builtin__min_cap ;
 
 }
 ;
 
 if ( cap > builtin__max_cap ) {
 
 cap  =  builtin__max_cap ;
 
 }
 ;
 
return  (hashmap) { .cap =  cap , .elm_size =  4 , .table =  make ( cap , cap , sizeof( hashmapentry) ) , .keys =  new_array(0, 1, sizeof( string )) , .nr_collisions =  0 } ;
 
 }
 void hashmap_set(hashmap* m, string key, int val) {
 
int hash= ((int)( b_fabs ( string_hash ( key ) ) ) ) ;
 
int idx= hash % m ->cap ;
 
 if ( ( *(hashmapentry*) array_get(  m ->table , idx) ) .key .len != 0 ) {
 
 m ->nr_collisions ++ ;
 
hashmapentry* e= & ( *(hashmapentry*) array_get(  m ->table , idx) ) ;
 
 while ( e ->next != 0 ) {
 
 
 e  =  e ->next ;
 
 }
 ;
 
 e ->next  =  (hashmapentry*)memdup(&(hashmapentry)  { key , val , 0 } , sizeof(hashmapentry)) ;
 
 }
  else { 
 
array_set(&/*q*/ m ->table , idx , & (hashmapentry []) {  (hashmapentry) { key , val , 0 } }) ;
 
 }
 ;
 
 }
 int hashmap_get(hashmap* m, string key) {
 
int hash= ((int)( b_fabs ( string_hash ( key ) ) ) ) ;
 
int idx= hash % m ->cap ;
 
hashmapentry* e= & ( *(hashmapentry*) array_get(  m ->table , idx) ) ;
 
 while ( e ->next != 0 ) {
 
 
 if (string_eq( e ->key , key ) ) {
 
return  e ->val ;
 
 }
 ;
 
 e  =  e ->next ;
 
 }
 ;
 
return  e ->val ;
 
 }
 static inline f64 b_fabs(int v) {
 
return  ( v < 0 ) ? ( - v ) : ( v ) ;
 
 }
 string ptr_str(void* ptr) {
 
byte* buf= v_malloc ( sizeof( double) * 5 + 1 ) ;
 
 sprintf ( ((char*)( buf ) ) ,  "%p" ,  ptr ) ;
 
return  tos ( buf , vstrlen ( buf ) ) ;
 
 }
 string int_str(int nn) {
 
int n= nn ;
 
 if ( n == 0 ) {
 
return  tos3("0") ;
 
 }
 ;
 
int max= 16 ;
 
byte* buf= v_calloc ( max ) ;
 
int len= 0 ;
 
bool is_neg= 0 ;
 
 if ( n < 0 ) {
 
 n  =  - n ;
 
 is_neg  =  1 ;
 
 }
 ;
 
 while ( n > 0 ) {
 
 
int d= n % 10 ;
 
 buf [/*ptr*/ max - len - 1 ]/*rbyte 1*/  =  d + ((int)( '0' ) ) ;
 
 len ++ ;
 
 n  =  n / 10 ;
 
 }
 ;
 
 if ( is_neg ) {
 
 buf [/*ptr*/ max - len - 1 ]/*rbyte 1*/  =  '-' ;
 
 len ++ ;
 
 }
 ;
 
return  tos ( buf + max - len , len ) ;
 
 }
 string u32_str(u32 nn) {
 
u32 n= nn ;
 
 if ( n == ((u32)( 0 ) ) ) {
 
return  tos3("0") ;
 
 }
 ;
 
int max= 16 ;
 
byte* buf= v_malloc ( max ) ;
 
int len= 0 ;
 
 while ( n > ((u32)( 0 ) ) ) {
 
 
u32 d= n % ((u32)( 10 ) ) ;
 
 buf [/*ptr*/ max - len - 1 ]/*rbyte 1*/  =  d + ((u32)( '0' ) ) ;
 
 len ++ ;
 
 n  =  n / ((u32)( 10 ) ) ;
 
 }
 ;
 
return  tos ( buf + max - len , len ) ;
 
 }
 string i64_str(i64 nn) {
 
i64 n= nn ;
 
 if ( n == ((i64)( 0 ) ) ) {
 
return  tos3("0") ;
 
 }
 ;
 
int max= 32 ;
 
byte* buf= v_malloc ( max ) ;
 
int len= 0 ;
 
bool is_neg= 0 ;
 
 if ( n < ((i64)( 0 ) ) ) {
 
 n  =  - n ;
 
 is_neg  =  1 ;
 
 }
 ;
 
 while ( n > ((i64)( 0 ) ) ) {
 
 
int d= ((int)( n % ((i64)( 10 ) ) ) ) ;
 
 buf [/*ptr*/ max - len - 1 ]/*rbyte 1*/  =  d + ((int)( '0' ) ) ;
 
 len ++ ;
 
 n  =  n / ((i64)( 10 ) ) ;
 
 }
 ;
 
 if ( is_neg ) {
 
 buf [/*ptr*/ max - len - 1 ]/*rbyte 1*/  =  '-' ;
 
 len ++ ;
 
 }
 ;
 
return  tos ( buf + max - len , len ) ;
 
 }
 string u64_str(u64 nn) {
 
u64 n= nn ;
 
 if ( n == ((u64)( 0 ) ) ) {
 
return  tos3("0") ;
 
 }
 ;
 
int max= 32 ;
 
byte* buf= v_malloc ( max ) ;
 
int len= 0 ;
 
 while ( n > ((u64)( 0 ) ) ) {
 
 
u64 d= n % ((u64)( 10 ) ) ;
 
 buf [/*ptr*/ max - len - 1 ]/*rbyte 1*/  =  d + ((u64)( '0' ) ) ;
 
 len ++ ;
 
 n  =  n / ((u64)( 10 ) ) ;
 
 }
 ;
 
return  tos ( buf + max - len , len ) ;
 
 }
 string bool_str(bool b) {
 
 if ( b ) {
 
return  tos3("true") ;
 
 }
 ;
 
return  tos3("false") ;
 
 }
 string int_hex(int n) {
 
int len= ( n >= 0 ) ? ( int_str ( n ) .len + 3 ) : ( 11 ) ;
 
byte* hex= v_malloc ( len ) ;
 
int count= ((int)( sprintf ( ((char*)( hex ) ) ,  "0x%x" ,  n ) ) ) ;
 
return  tos ( hex , count ) ;
 
 }
 string i64_hex(i64 n) {
 
int len= ( n >= ((i64)( 0 ) ) ) ? ( i64_str ( n ) .len + 3 ) : ( 19 ) ;
 
byte* hex= v_malloc ( len ) ;
 
int count= ((int)( sprintf ( ((char*)( hex ) ) ,  "0x%" PRIx64 ,  n ) ) ) ;
 
return  tos ( hex , count ) ;
 
 }
 bool array_byte_contains(array_byte a, byte val) {
 
 array_byte tmp1 =  a;
 for (int tmp2 = 0; tmp2 < tmp1.len; tmp2++) {
 byte aa = ((byte *) tmp1 . data)[tmp2];
 
 
 if ( aa == val ) {
 
return  1 ;
 
 }
 ;
 
 }
 ;
 
return  0 ;
 
 }
 string rune_str(rune c) {
 
int fst_byte= ((int)( c ) )  >>  8 * 3 & 0xff ;
 
int len= utf8_char_len ( fst_byte ) ;
 
string str= (string) { .len =  len , .str =  v_malloc ( len + 1 ) } ;
 
 for (
int i= 0  ;  i < len  ;  i ++ ) { 
 
 
 str .str [/*ptr*/ i ]/*rbyte 1*/  =  ((int)( c ) )  >>  8 * ( 3 - i ) & 0xff ;
 
 }
 ;
 
 str .str[ len ]/*rbyte 1*/  =  '\0' ;
 
return  str ;
 
 }
 string byte_str(byte c) {
 
string str= (string) { .len =  1 , .str =  v_malloc ( 2 ) } ;
 
 str .str [/*ptr*/ 0 ]/*rbyte 1*/  =  c ;
 
 str .str [/*ptr*/ 1 ]/*rbyte 1*/  =  '\0' ;
 
return  str ;
 
 }
 bool byte_is_capital(byte c) {
 
return  c >= 'A'  &&  c <= 'Z' ;
 
 }
 array_byte array_byte_clone(array_byte b) {
 
array_byte res= array_repeat (new_array_from_c_array(1, 1, sizeof(byte), EMPTY_ARRAY_OF_ELEMS( byte, 1 ) {  ((byte)( 0 ) )  }) , b .len ) ;
 
 for (
int i= 0  ;  i < b .len  ;  i ++ ) { 
 
 
array_set(&/*q*/ res , i , & (byte []) {  ( *(byte*) array_get(  b , i) ) }) ;
 
 }
 ;
 
return  res ;
 
 }
 map new_map(int cap, int elm_size) {
 
map res= (map) { .element_size =  elm_size , .root =  0 , .size =  0 } ;
 
return  res ;
 
 }
 map new_map_init(int cap, int elm_size, string* keys, void* vals) {
 
map res= (map) { .element_size =  elm_size , .root =  0 , .size =  0 } ;
 
 int tmp1 =  0;
 ;
for (int tmp2 = tmp1; tmp2 <  cap; tmp2++) {
 int i = tmp2;
 
 
 map_set (& /* ? */ res , keys [/*ptr*/ i ]/*rstring 0*/ ,(byte*) vals + i * elm_size ) ;
 
 }
 ;
 
return  res ;
 
 }
 mapnode* new_node(string key, void* val, int element_size) {
 
mapnode* new_e= (mapnode*)memdup(&(mapnode)  { .key =  key , .val =  v_malloc ( element_size ) , .left =  0 , .right =  0 , .is_empty =  0 , } , sizeof(mapnode)) ;
 
 memcpy ( new_e ->val ,  val ,  element_size ) ;
 
return  new_e ;
 
 }
 void map_insert(map* m, mapnode* n, string key, void* val) {
 
 if (string_eq( n ->key , key ) ) {
 
 memcpy ( n ->val ,  val ,  m ->element_size ) ;
 
 
 return ;
 
 }
 ;
 
 if (string_gt( n ->key , key ) ) {
 
 if ( n ->left == 0 ) {
 
 n ->left  =  new_node ( key , val , m ->element_size ) ;
 
 m ->size ++ ;
 
 }
  else { 
 
 map_insert ( m , n ->left , key , val ) ;
 
 }
 ;
 
 
 return ;
 
 }
 ;
 
 if ( n ->right == 0 ) {
 
 n ->right  =  new_node ( key , val , m ->element_size ) ;
 
 m ->size ++ ;
 
 }
  else { 
 
 map_insert ( m , n ->right , key , val ) ;
 
 }
 ;
 
 }
 bool mapnode_find(mapnode* n, string key, void* out, int element_size) {
 
 if (string_eq( n ->key , key ) ) {
 
 memcpy ( out ,  n ->val ,  element_size ) ;
 
return  1 ;
 
 }
  else  if (string_gt( n ->key , key ) ) {
 
 if ( n ->left == 0 ) {
 
return  0 ;
 
 }
  else { 
 
return  mapnode_find (& /* ? */* n ->left , key , out , element_size ) ;
 
 }
 ;
 
 }
  else { 
 
 if ( n ->right == 0 ) {
 
return  0 ;
 
 }
  else { 
 
return  mapnode_find (& /* ? */* n ->right , key , out , element_size ) ;
 
 }
 ;
 
 }
 ;
 
 }
 bool mapnode_find2(mapnode* n, string key, int element_size) {
 
 if (string_eq( n ->key , key ) ) {
 
return  1 ;
 
 }
  else  if (string_gt( n ->key , key ) ) {
 
 if ( isnil ( n ->left ) ) {
 
return  0 ;
 
 }
  else { 
 
return  mapnode_find2 (& /* ? */* n ->left , key , element_size ) ;
 
 }
 ;
 
 }
  else { 
 
 if ( isnil ( n ->right ) ) {
 
return  0 ;
 
 }
  else { 
 
return  mapnode_find2 (& /* ? */* n ->right , key , element_size ) ;
 
 }
 ;
 
 }
 ;
 
 }
 void map_set(map* m, string key, void* val) {
 
 if ( isnil ( m ->root ) ) {
 
 m ->root  =  new_node ( key , val , m ->element_size ) ;
 
 m ->size ++ ;
 
 
 return ;
 
 }
 ;
 
 map_insert ( m , m ->root , key , val ) ;
 
 }
 int preorder_keys(mapnode* node, array_string* keys, int key_i) {
 
int i= key_i ;
 
 if ( ! node ->is_empty ) {
 
array_set( keys , i , & (string []) {  node ->key }) ;
 
 i ++ ;
 
 }
 ;
 
 if ( ! isnil ( node ->left ) ) {
 
 i  =  preorder_keys ( node ->left , keys , i ) ;
 
 }
 ;
 
 if ( ! isnil ( node ->right ) ) {
 
 i  =  preorder_keys ( node ->right , keys , i ) ;
 
 }
 ;
 
return  i ;
 
 }
 array_string map_keys(map* m) {
 
array_string keys= array_repeat (new_array_from_c_array(1, 1, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 1 ) {  tos3("")  }) , m ->size ) ;
 
 if ( isnil ( m ->root ) ) {
 
return  keys ;
 
 }
 ;
 
 preorder_keys ( m ->root ,& /*111*/ (array[]){ keys }[0]  , 0 ) ;
 
return  keys ;
 
 }
 bool map_get(map m, string key, void* out) {
 
 if ( m .root == 0 ) {
 
return  0 ;
 
 }
 ;
 
return  mapnode_find (& /* ? */* m .root , key , out , m .element_size ) ;
 
 }
 void v_mapnode_delete(mapnode* n, string key, int element_size) {
 
 if (string_eq( n ->key , key ) ) {
 
 memset ( n ->val ,  0 ,  element_size ) ;
 
 n ->is_empty  =  1 ;
 
 
 return ;
 
 }
  else  if (string_gt( n ->key , key ) ) {
 
 if ( isnil ( n ->left ) ) {
 
 
 return ;
 
 }
  else { 
 
 v_mapnode_delete ( n ->left , key , element_size ) ;
 
 }
 ;
 
 }
  else { 
 
 if ( isnil ( n ->right ) ) {
 
 
 return ;
 
 }
  else { 
 
 v_mapnode_delete ( n ->right , key , element_size ) ;
 
 }
 ;
 
 }
 ;
 
 }
 void v_map_delete(map* m, string key) {
 
 if ( map_exists (* m , key ) ) {
 
 v_mapnode_delete ( m ->root , key , m ->element_size ) ;
 
 m ->size -- ;
 
 }
 ;
 
 }
 bool map_exists(map m, string key) {
 
return  ! isnil ( m .root )  &&  mapnode_find2 (& /* ? */* m .root , key , m .element_size ) ;
 
 }
 void map_print(map m) {
 
 println ( tos3("<<<<<<<<") ) ;
 
 println ( tos3(">>>>>>>>>>") ) ;
 
 }
 void v_mapnode_free(mapnode* n) {
 
 if ( n ->val != 0 ) {
 
 v_free ( n ->val ) ;
 
 }
 ;
 
 if ( n ->left != 0 ) {
 
 v_mapnode_free ( n ->left ) ;
 
 }
 ;
 
 if ( n ->right != 0 ) {
 
 v_mapnode_free ( n ->right ) ;
 
 }
 ;
 
 v_free ( n ) ;
 
 }
 void v_map_free(map* m) {
 
 if ( m ->root == 0 ) {
 
 
 return ;
 
 }
 ;
 
 v_mapnode_free ( m ->root ) ;
 
 }
 string map_string_str(map_string m) {
 
 if ( m .size == 0 ) {
 
return  tos3("{}") ;
 
 }
 ;
 
strings__Builder sb= strings__new_builder ( 50 ) ;
 
 strings__Builder_writeln (& /* ? */ sb , tos3("{") ) ;
 
 map_string tmp3 =  m;
 array_string keys_tmp3 = map_keys(& tmp3 ); 
 for (int l = 0; l < keys_tmp3 .len; l++) {
 string key = ((string*)keys_tmp3 .data)[l];
 string val = {0}; map_get(tmp3, key, & val);
 
 
 strings__Builder_writeln (& /* ? */ sb , _STR("  \"%.*s\" => \"%.*s\"", key.len, key.str, val.len, val.str) ) ;
 
 }
 ;
 
 strings__Builder_writeln (& /* ? */ sb , tos3("}") ) ;
 
return  strings__Builder_str ( sb ) ;
 
 }
 Option opt_ok(void* data, int size) {
 
 if ( size >= 300 ) {
 
 v_panic ( _STR("option size too big: %d (max is 300), this is a temporary limit", size) ) ;
 
 }
 ;
 
Option res= (Option) { .ok =  1 , .error =  tos3("") , .ecode =  0 , .is_none =  0 } ;
 
 memcpy ( res .data ,  data ,  size ) ;
 
return  res ;
 
 }
 Option opt_none() {
 
return  (Option) { .is_none =  1 , .error =  tos3("") , .ecode =  0 , .ok =  0 , } ;
 
 }
 Option v_error(string s) {
 
return  (Option) { .error =  s , .ecode =  0 , .ok =  0 , .is_none =  0 } ;
 
 }
 Option error_with_code(string s, int code) {
 
return  (Option) { .error =  s , .ecode =  code , .ok =  0 , .is_none =  0 } ;
 
 }
 int vstrlen(byte* s) {
 
return  strlen ( ((char*)( s ) ) ) ;
 
 }
 string tos(byte* s, int len) {
 
 if ( s == 0 ) {
 
 v_panic ( tos3("tos(): nil string") ) ;
 
 }
 ;
 
return  (string) { .str =  s , .len =  len } ;
 
 }
 string tos_clone(byte* s) {
 
 if ( s == 0 ) {
 
 v_panic ( tos3("tos: nil string") ) ;
 
 }
 ;
 
return  string_clone ( tos2 ( s ) ) ;
 
 }
 string tos2(byte* s) {
 
 if ( s == 0 ) {
 
 v_panic ( tos3("tos2: nil string") ) ;
 
 }
 ;
 
return  (string) { .str =  s , .len =  vstrlen ( s ) } ;
 
 }
 string tos3(char* s) {
 
 if ( s == 0 ) {
 
 v_panic ( tos3("tos3: nil string") ) ;
 
 }
 ;
 
return  (string) { .str =  ((byteptr)( s ) ) , .len =  strlen ( s ) } ;
 
 }
 string string_clone(string a) {
 
string b= (string) { .len =  a .len , .str =  v_malloc ( a .len + 1 ) } ;
 
 for (
int i= 0  ;  i < a .len  ;  i ++ ) { 
 
 
 b .str[ i ]/*rbyte 1*/  =  a .str[ i ]/*rbyte 0*/ ;
 
 }
 ;
 
 b .str[ a .len ]/*rbyte 1*/  =  '\0' ;
 
return  b ;
 
 }
 string string_replace(string s, string rep, string with) {
 
 if ( s .len == 0  ||  rep .len == 0 ) {
 
return  s ;
 
 }
 ;
 
array_int idxs=new_array_from_c_array(0, 0, sizeof(int), EMPTY_ARRAY_OF_ELEMS( int, 0 ) {   TCCSKIP(0) }) ;
 
string rem= s ;
 
int rstart= 0 ;
 
 while (1) { 
 
int i= string_index ( rem , rep ) ;
 
 if ( i < 0 ) {
 
 break
 ;
 
 }
 ;
 
_PUSH(& idxs , ( /*typ = array_int   tmp_typ=int*/ rstart + i ), tmp1, int) ;
 
 i  +=  rep .len ;
 
 rstart  +=  i ;
 
 rem  =  string_substr ( rem , i , rem .len ) ;
 
 }
 ;
 
 if ( idxs .len == 0 ) {
 
return  s ;
 
 }
 ;
 
int new_len= s .len + idxs .len * ( with .len - rep .len ) ;
 
byte* b= v_malloc ( new_len + 1 ) ;
 
int idx_pos= 0 ;
 
int cur_idx= ( *(int*) array_get(  idxs , idx_pos) ) ;
 
int b_i= 0 ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
 if ( i == cur_idx ) {
 
 for (
int j= 0  ;  j < with .len  ;  j ++ ) { 
 
 
 b [/*ptr*/ b_i ]/*rbyte 1*/  =  with .str[ j ]/*rbyte 0*/ ;
 
 b_i ++ ;
 
 }
 ;
 
 i  +=  rep .len - 1 ;
 
 idx_pos ++ ;
 
 if ( idx_pos < idxs .len ) {
 
 cur_idx  =  ( *(int*) array_get(  idxs , idx_pos) ) ;
 
 }
 ;
 
 }
  else { 
 
 b [/*ptr*/ b_i ]/*rbyte 1*/  =  s .str[ i ]/*rbyte 0*/ ;
 
 b_i ++ ;
 
 }
 ;
 
 }
 ;
 
 b [/*ptr*/ new_len ]/*rbyte 1*/  =  '\0' ;
 
return  tos ( b , new_len ) ;
 
 }
 int v_string_int(string s) {
 
bool neg= 0 ;
 
int i= 0 ;
 
 if ( s .str[ 0 ]/*rbyte 0*/ == '-' ) {
 
 neg  =  1 ;
 
 i ++ ;
 
 }
  else  if ( s .str[ 0 ]/*rbyte 0*/ == '+' ) {
 
 i ++ ;
 
 }
 ;
 
int n= 0 ;
 
 while ( isdigit ( s .str[ i ]/*rbyte 0*/ ) ) {
 
 
 n  =  10 * n - ((int)( s .str[ i ]/*rbyte 0*/ - '0' ) ) ;
 
 i ++ ;
 
 }
 ;
 
return  ( neg ) ? ( n ) : ( - n ) ;
 
 }
 i64 string_i64(string s) {
 
return  strconv__parse_int ( s , 0 , 64 ) ;
 
 }
 f32 string_f32(string s) {
 
return  atof ( ((char*)( s .str ) ) ) ;
 
 }
 f64 string_f64(string s) {
 
return  atof ( ((char*)( s .str ) ) ) ;
 
 }
 u32 string_u32(string s) {
 
bool neg= 0 ;
 
int i= 0 ;
 
 if ( s .str[ 0 ]/*rbyte 0*/ == '-' ) {
 
 neg  =  1 ;
 
 i ++ ;
 
 }
  else  if ( s .str[ 0 ]/*rbyte 0*/ == '+' ) {
 
 i ++ ;
 
 }
 ;
 
u32 n= ((u32)( 0 ) ) ;
 
 while ( isdigit ( s .str[ i ]/*rbyte 0*/ ) ) {
 
 
 n  =  ((u32)( 10 ) ) * n - ((u32)( s .str[ i ]/*rbyte 0*/ - '0' ) ) ;
 
 i ++ ;
 
 }
 ;
 
return  ( neg ) ? ( n ) : ( - n ) ;
 
 }
 u64 string_u64(string s) {
 
return  strconv__parse_uint ( s , 0 , 64 ) ;
 
 }
 bool string_eq(string s, string a) {
 
 if ( isnil ( s .str ) ) {
 
 v_panic ( tos3("string.eq(): nil string") ) ;
 
 }
 ;
 
 if ( s .len != a .len ) {
 
return  0 ;
 
 }
 ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
 if ( s .str[ i ]/*rbyte 0*/ != a .str[ i ]/*rbyte 0*/ ) {
 
return  0 ;
 
 }
 ;
 
 }
 ;
 
return  1 ;
 
 }
 bool string_ne(string s, string a) {
 
return  ! string_eq ( s , a ) ;
 
 }
 bool string_lt(string s, string a) {
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
 if ( i >= a .len  ||  s .str[ i ]/*rbyte 0*/ > a .str[ i ]/*rbyte 0*/ ) {
 
return  0 ;
 
 }
  else  if ( s .str[ i ]/*rbyte 0*/ < a .str[ i ]/*rbyte 0*/ ) {
 
return  1 ;
 
 }
 ;
 
 }
 ;
 
 if ( s .len < a .len ) {
 
return  1 ;
 
 }
 ;
 
return  0 ;
 
 }
 bool string_le(string s, string a) {
 
return  string_lt ( s , a )  ||  string_eq ( s , a ) ;
 
 }
 bool string_gt(string s, string a) {
 
return  ! string_le ( s , a ) ;
 
 }
 bool string_ge(string s, string a) {
 
return  ! string_lt ( s , a ) ;
 
 }
 string string_add(string s, string a) {
 
int new_len= a .len + s .len ;
 
string res= (string) { .len =  new_len , .str =  v_malloc ( new_len + 1 ) } ;
 
 for (
int j= 0  ;  j < s .len  ;  j ++ ) { 
 
 
 res .str[ j ]/*rbyte 1*/  =  s .str[ j ]/*rbyte 0*/ ;
 
 }
 ;
 
 for (
int j= 0  ;  j < a .len  ;  j ++ ) { 
 
 
 res .str[ s .len + j ]/*rbyte 1*/  =  a .str[ j ]/*rbyte 0*/ ;
 
 }
 ;
 
 res .str[ new_len ]/*rbyte 1*/  =  '\0' ;
 
return  res ;
 
 }
 array_string string_split(string s, string delim) {
 
array_string res=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 if ( delim .len == 0 ) {
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ s ), tmp6, string) ;
 
return  res ;
 
 }
 ;
 
 if ( delim .len == 1 ) {
 
return  string_split_single ( s , delim .str[ 0 ]/*rbyte 0*/ ) ;
 
 }
 ;
 
int i= 0 ;
 
int start= 0 ;
 
 while ( i < s .len ) {
 
 
bool a= s .str[ i ]/*rbyte 0*/ == delim .str[ 0 ]/*rbyte 0*/ ;
 
int j= 1 ;
 
 while ( j < delim .len  &&  a ) {
 
 
 a  =  a  &&  s .str[ i + j ]/*rbyte 0*/ == delim .str[ j ]/*rbyte 0*/ ;
 
 j ++ ;
 
 }
 ;
 
bool last= i == s .len - 1 ;
 
 if ( a  ||  last ) {
 
 if ( last ) {
 
 i ++ ;
 
 }
 ;
 
string val= string_substr ( s , start , i ) ;
 
 if ( val .len > 0 ) {
 
 if ( string_starts_with ( val , delim ) ) {
 
 val  =  string_right ( val , delim .len ) ;
 
 }
 ;
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ string_trim_space ( val ) ), tmp7, string) ;
 
 }
 ;
 
 start  =  i ;
 
 }
 ;
 
 i ++ ;
 
 }
 ;
 
return  res ;
 
 }
 array_string string_split_single(string s, byte delim) {
 
array_string res=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 if ( ((int)( delim ) ) == 0 ) {
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ s ), tmp8, string) ;
 
return  res ;
 
 }
 ;
 
int i= 0 ;
 
int start= 0 ;
 
 while ( i < s .len ) {
 
 
bool is_delim= s .str[ i ]/*rbyte 0*/ == delim ;
 
bool last= i == s .len - 1 ;
 
 if ( is_delim  ||  last ) {
 
 if ( ! is_delim  &&  i == s .len - 1 ) {
 
 i ++ ;
 
 }
 ;
 
string val= string_substr ( s , start , i ) ;
 
 if ( val .len > 0 ) {
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ val ), tmp9, string) ;
 
 }
 ;
 
 start  =  i + 1 ;
 
 }
 ;
 
 i ++ ;
 
 }
 ;
 
return  res ;
 
 }
 array_string string_split_into_lines(string s) {
 
array_string res=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 if ( s .len == 0 ) {
 
return  res ;
 
 }
 ;
 
int start= 0 ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
bool last= i == s .len - 1 ;
 
 if ( ((int)( s .str[ i ]/*rbyte 0*/ ) ) == 10  ||  last ) {
 
 if ( last ) {
 
 i ++ ;
 
 }
 ;
 
string line= string_substr ( s , start , i ) ;
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ line ), tmp10, string) ;
 
 start  =  i + 1 ;
 
 }
 ;
 
 }
 ;
 
return  res ;
 
 }
 string string_left(string s, int n) {
 
 if ( n >= s .len ) {
 
return  s ;
 
 }
 ;
 
return  string_substr ( s , 0 , n ) ;
 
 }
 string string_right(string s, int n) {
 
 if ( n >= s .len ) {
 
return  tos3("") ;
 
 }
 ;
 
return  string_substr ( s , n , s .len ) ;
 
 }
 string string_substr2(string s, int start, int _end, bool end_max) {
 
int end= ( end_max ) ? ( s .len ) : ( _end ) ;
 
return  string_substr ( s , start , end ) ;
 
 }
 string string_substr(string s, int start, int end) {
 
 if ( start > end  ||  start > s .len  ||  end > s .len  ||  start < 0  ||  end < 0 ) {
 
 v_panic ( _STR("substr(%d, %d) out of bounds (len=%d)", start, end, s .len) ) ;
 
 }
 ;
 
int len= end - start ;
 
string res= (string) { .len =  len , .str =  v_malloc ( len + 1 ) } ;
 
 for (
int i= 0  ;  i < len  ;  i ++ ) { 
 
 
 res .str [/*ptr*/ i ]/*rbyte 1*/  =  s .str [/*ptr*/ start + i ]/*rbyte 0*/ ;
 
 }
 ;
 
 res .str [/*ptr*/ len ]/*rbyte 1*/  =  '\0' ;
 
return  res ;
 
 }
 int string_index(string s, string p) {
 
 if ( p .len > s .len ) {
 
return  - 1 ;
 
 }
 ;
 
int i= 0 ;
 
 while ( i < s .len ) {
 
 
int j= 0 ;
 
 while ( j < p .len  &&  s .str[ i + j ]/*rbyte 0*/ == p .str[ j ]/*rbyte 0*/ ) {
 
 
 j ++ ;
 
 }
 ;
 
 if ( j == p .len ) {
 
return  i ;
 
 }
 ;
 
 i ++ ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int string_index_kmp(string s, string p) {
 
 if ( p .len > s .len ) {
 
return  - 1 ;
 
 }
 ;
 
array_int prefix= array_repeat (new_array_from_c_array(1, 1, sizeof(int), EMPTY_ARRAY_OF_ELEMS( int, 1 ) {  0  }) , p .len ) ;
 
int j= 0 ;
 
 for (
int i= 1  ;  i < p .len  ;  i ++ ) { 
 
 
 while ( p .str[ j ]/*rbyte 0*/ != p .str[ i ]/*rbyte 0*/  &&  j > 0 ) {
 
 
 j  =  ( *(int*) array_get(  prefix , j - 1) ) ;
 
 }
 ;
 
 if ( p .str[ j ]/*rbyte 0*/ == p .str[ i ]/*rbyte 0*/ ) {
 
 j ++ ;
 
 }
 ;
 
array_set(&/*q*/ prefix , i , & (int []) {  j }) ;
 
 }
 ;
 
 j  =  0 ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
 while ( p .str[ j ]/*rbyte 0*/ != s .str[ i ]/*rbyte 0*/  &&  j > 0 ) {
 
 
 j  =  ( *(int*) array_get(  prefix , j - 1) ) ;
 
 }
 ;
 
 if ( p .str[ j ]/*rbyte 0*/ == s .str[ i ]/*rbyte 0*/ ) {
 
 j ++ ;
 
 }
 ;
 
 if ( j == p .len ) {
 
return  i - p .len + 1 ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int string_index_any(string s, string chars) {
 
 string tmp15 =  chars;
 array_byte bytes_tmp15 = string_bytes( tmp15 );
 ;
for (int tmp16 = 0; tmp16 < tmp15 .len; tmp16 ++) {
 byte c = ((byte *) bytes_tmp15 . data)[tmp16];
 
 
int index= string_index ( s , byte_str ( c ) ) ;
 
 if ( index != - 1 ) {
 
return  index ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int string_last_index(string s, string p) {
 
 if ( p .len > s .len ) {
 
return  - 1 ;
 
 }
 ;
 
int i= s .len - p .len ;
 
 while ( i >= 0 ) {
 
 
int j= 0 ;
 
 while ( j < p .len  &&  s .str[ i + j ]/*rbyte 0*/ == p .str[ j ]/*rbyte 0*/ ) {
 
 
 j ++ ;
 
 }
 ;
 
 if ( j == p .len ) {
 
return  i ;
 
 }
 ;
 
 i -- ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int string_index_after(string s, string p, int start) {
 
 if ( p .len > s .len ) {
 
return  - 1 ;
 
 }
 ;
 
int strt= start ;
 
 if ( start < 0 ) {
 
 strt  =  0 ;
 
 }
 ;
 
 if ( start >= s .len ) {
 
return  - 1 ;
 
 }
 ;
 
int i= strt ;
 
 while ( i < s .len ) {
 
 
int j= 0 ;
 
int ii= i ;
 
 while ( j < p .len  &&  s .str[ ii ]/*rbyte 0*/ == p .str[ j ]/*rbyte 0*/ ) {
 
 
 j ++ ;
 
 ii ++ ;
 
 }
 ;
 
 if ( j == p .len ) {
 
return  i ;
 
 }
 ;
 
 i ++ ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int string_index_byte(string s, byte c) {
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
 if ( s .str[ i ]/*rbyte 0*/ == c ) {
 
return  i ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int string_last_index_byte(string s, byte c) {
 
 for (
int i= s .len - 1  ;  i >= 0  ;  i -- ) { 
 
 
 if ( s .str[ i ]/*rbyte 0*/ == c ) {
 
return  i ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int string_count(string s, string substr) {
 
 if ( s .len == 0  ||  substr .len == 0 ) {
 
return  0 ;
 
 }
 ;
 
 if ( substr .len > s .len ) {
 
return  0 ;
 
 }
 ;
 
int n= 0 ;
 
int i= 0 ;
 
 while (1) { 
 
 i  =  string_index_after ( s , substr , i ) ;
 
 if ( i == - 1 ) {
 
return  n ;
 
 }
 ;
 
 i  +=  substr .len ;
 
 n ++ ;
 
 }
 ;
 
return  0 ;
 
 }
 bool string_contains(string s, string p) {
 
bool res= string_index ( s , p ) > 0 - 1 ;
 
return  res ;
 
 }
 bool string_starts_with(string s, string p) {
 
bool res= string_index ( s , p ) == 0 ;
 
return  res ;
 
 }
 bool string_ends_with(string s, string p) {
 
 if ( p .len > s .len ) {
 
return  0 ;
 
 }
 ;
 
bool res= string_last_index ( s , p ) == s .len - p .len ;
 
return  res ;
 
 }
 string string_to_lower(string s) {
 
byte* b= v_malloc ( s .len + 1 ) ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
 b [/*ptr*/ i ]/*rbyte 1*/  =  tolower ( s .str [/*ptr*/ i ]/*rbyte 0*/ ) ;
 
 }
 ;
 
return  tos ( b , s .len ) ;
 
 }
 string string_to_upper(string s) {
 
byte* b= v_malloc ( s .len + 1 ) ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
 b [/*ptr*/ i ]/*rbyte 1*/  =  toupper ( s .str [/*ptr*/ i ]/*rbyte 0*/ ) ;
 
 }
 ;
 
return  tos ( b , s .len ) ;
 
 }
 string string_capitalize(string s) {
 
string sl= string_to_lower ( s ) ;
 
string cap=string_add( string_to_upper ( byte_str ( sl .str[ 0 ]/*rbyte 0*/ ) ) , string_right ( sl , 1 ) ) ;
 
return  cap ;
 
 }
 string string_title(string s) {
 
array_string words= string_split ( s , tos3(" ") ) ;
 
array_string tit=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 array_string tmp17 =  words;
 for (int tmp18 = 0; tmp18 < tmp17.len; tmp18++) {
 string word = ((string *) tmp17 . data)[tmp18];
 
 
_PUSH(& tit , ( /*typ = array_string   tmp_typ=string*/ string_capitalize ( word ) ), tmp19, string) ;
 
 }
 ;
 
string title= array_string_join ( tit , tos3(" ") ) ;
 
return  title ;
 
 }
 string string_find_between(string s, string start, string end) {
 
int start_pos= string_index ( s , start ) ;
 
 if ( start_pos == - 1 ) {
 
return  tos3("") ;
 
 }
 ;
 
string val= string_right ( s , start_pos + start .len ) ;
 
int end_pos= string_last_index ( val , end ) ;
 
 if ( end_pos == - 1 ) {
 
return  val ;
 
 }
 ;
 
return  string_left ( val , end_pos ) ;
 
 }
 bool array_string_contains(array_string ar, string val) {
 
 array_string tmp20 =  ar;
 for (int tmp21 = 0; tmp21 < tmp20.len; tmp21++) {
 string s = ((string *) tmp20 . data)[tmp21];
 
 
 if (string_eq( s , val ) ) {
 
return  1 ;
 
 }
 ;
 
 }
 ;
 
return  0 ;
 
 }
 bool array_int_contains(array_int ar, int val) {
 
 array_int tmp22 =  ar;
 for (int i = 0; i < tmp22.len; i++) {
 int s = ((int *) tmp22 . data)[i];
 
 
 if ( s == val ) {
 
return  1 ;
 
 }
 ;
 
 }
 ;
 
return  0 ;
 
 }
 bool byte_is_space(byte c) {
 
return  c  == ' '  ||  c ==  '\n'  ||  c ==  '\t'  ||  c ==  '\v'  ||  c ==  '\f'  ||  c ==  '\r' ;
 
 }
 string string_trim_space(string s) {
 
return  string_trim ( s , tos3(" \n\t\v\f\r") ) ;
 
 }
 string string_trim(string s, string cutset) {
 
 if ( s .len < 1  ||  cutset .len < 1 ) {
 
return  s ;
 
 }
 ;
 
array_byte cs_arr= string_bytes ( cutset ) ;
 
int pos_left= 0 ;
 
int pos_right= s .len - 1 ;
 
bool cs_match= 1 ;
 
 while ( pos_left <= s .len  &&  pos_right >= - 1  &&  cs_match ) {
 
 
 cs_match  =  0 ;
 
 if (_IN(byte, ( s .str[ pos_left ]/*rbyte 0*/ ),  cs_arr ) ) {
 
 pos_left ++ ;
 
 cs_match  =  1 ;
 
 }
 ;
 
 if (_IN(byte, ( s .str[ pos_right ]/*rbyte 0*/ ),  cs_arr ) ) {
 
 pos_right -- ;
 
 cs_match  =  1 ;
 
 }
 ;
 
 if ( pos_left > pos_right ) {
 
return  tos3("") ;
 
 }
 ;
 
 }
 ;
 
return  string_substr ( s , pos_left , pos_right + 1 ) ;
 
 }
 string string_trim_left(string s, string cutset) {
 
 if ( s .len < 1  ||  cutset .len < 1 ) {
 
return  s ;
 
 }
 ;
 
array_byte cs_arr= string_bytes ( cutset ) ;
 
int pos= 0 ;
 
 while ( pos <= s .len  && _IN(byte, ( s .str[ pos ]/*rbyte 0*/ ),  cs_arr ) ) {
 
 
 pos ++ ;
 
 }
 ;
 
return  string_right ( s , pos ) ;
 
 }
 string string_trim_right(string s, string cutset) {
 
 if ( s .len < 1  ||  cutset .len < 1 ) {
 
return  s ;
 
 }
 ;
 
array_byte cs_arr= string_bytes ( cutset ) ;
 
int pos= s .len - 1 ;
 
 while ( pos >= - 1  && _IN(byte, ( s .str[ pos ]/*rbyte 0*/ ),  cs_arr ) ) {
 
 
 pos -- ;
 
 }
 ;
 
return  string_left ( s , pos + 1 ) ;
 
 }
 int compare_strings(string* a, string* b) {
 
 if ( string_lt (* a ,* b ) ) {
 
return  - 1 ;
 
 }
 ;
 
 if ( string_gt (* a ,* b ) ) {
 
return  1 ;
 
 }
 ;
 
return  0 ;
 
 }
 int compare_strings_by_len(string* a, string* b) {
 
 if ( a ->len < b ->len ) {
 
return  - 1 ;
 
 }
 ;
 
 if ( a ->len > b ->len ) {
 
return  1 ;
 
 }
 ;
 
return  0 ;
 
 }
 int compare_lower_strings(string* a, string* b) {
 
string aa= string_to_lower (* a ) ;
 
string bb= string_to_lower (* b ) ;
 
return  compare_strings (& /*112 EXP:"string*" GOT:"string" */ aa ,& /*112 EXP:"string*" GOT:"string" */ bb ) ;
 
 }
 void array_string_sort(array_string* s) {
 
 array_sort_with_compare ( s ,& /*112 EXP:"void*" GOT:"fn (string*,string*) int" */ compare_strings ) ;
 
 }
 void array_string_sort_ignore_case(array_string* s) {
 
 array_sort_with_compare ( s ,& /*112 EXP:"void*" GOT:"fn (string*,string*) int" */ compare_lower_strings ) ;
 
 }
 void array_string_sort_by_len(array_string* s) {
 
 array_sort_with_compare ( s ,& /*112 EXP:"void*" GOT:"fn (string*,string*) int" */ compare_strings_by_len ) ;
 
 }
 ustring string_ustring(string s) {
 
ustring res= (ustring) { .s =  s , .runes =  new_array ( 0 , s .len , sizeof( int) ) , .len =  0 } ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
int char_len= utf8_char_len ( s .str [/*ptr*/ i ]/*rbyte 0*/ ) ;
 
_PUSH(& res .runes , ( /*typ = array_int   tmp_typ=int*/ i ), tmp23, int) ;
 
 i  +=  char_len - 1 ;
 
 res .len ++ ;
 
 }
 ;
 
return  res ;
 
 }
 ustring string_ustring_tmp(string s) {
 
 if ( g_ustring_runes .len == 0 ) {
 
 g_ustring_runes  =  new_array ( 0 , 128 , sizeof( int) ) ;
 
 }
 ;
 
ustring res= (ustring) { .s =  s , .runes =  new_array(0, 1, sizeof( int )) , .len =  0 } ;
 
 res .runes  =  g_ustring_runes ;
 
 res .runes .len  =  s .len ;
 
int j= 0 ;
 
 for (
int i= 0  ;  i < s .len  ;  i ++ ) { 
 
 
int char_len= utf8_char_len ( s .str [/*ptr*/ i ]/*rbyte 0*/ ) ;
 
array_set(&/*q*/ res .runes , j , & (int []) {  i }) ;
 
 j ++ ;
 
 i  +=  char_len - 1 ;
 
 res .len ++ ;
 
 }
 ;
 
return  res ;
 
 }
 bool ustring_eq(ustring u, ustring a) {
 
 if ( u .len != a .len  || string_ne( u .s , a .s ) ) {
 
return  0 ;
 
 }
 ;
 
return  1 ;
 
 }
 bool ustring_ne(ustring u, ustring a) {
 
return  ! ustring_eq ( u , a ) ;
 
 }
 bool ustring_lt(ustring u, ustring a) {
 
return string_lt( u .s , a .s ) ;
 
 }
 bool ustring_le(ustring u, ustring a) {
 
return  ustring_lt ( u , a )  ||  ustring_eq ( u , a ) ;
 
 }
 bool ustring_gt(ustring u, ustring a) {
 
return  ! ustring_le ( u , a ) ;
 
 }
 bool ustring_ge(ustring u, ustring a) {
 
return  ! ustring_lt ( u , a ) ;
 
 }
 ustring ustring_add(ustring u, ustring a) {
 
ustring res= (ustring) { .s = string_add( u .s , a .s ) , .runes =  new_array ( 0 , u .s .len + a .s .len , sizeof( int) ) , .len =  0 } ;
 
int j= 0 ;
 
 for (
int i= 0  ;  i < u .s .len  ;  i ++ ) { 
 
 
int char_len= utf8_char_len ( u .s .str [/*ptr*/ i ]/*rbyte 0*/ ) ;
 
_PUSH(& res .runes , ( /*typ = array_int   tmp_typ=int*/ j ), tmp24, int) ;
 
 i  +=  char_len - 1 ;
 
 j  +=  char_len ;
 
 res .len ++ ;
 
 }
 ;
 
 for (
int i= 0  ;  i < a .s .len  ;  i ++ ) { 
 
 
int char_len= utf8_char_len ( a .s .str [/*ptr*/ i ]/*rbyte 0*/ ) ;
 
_PUSH(& res .runes , ( /*typ = array_int   tmp_typ=int*/ j ), tmp25, int) ;
 
 i  +=  char_len - 1 ;
 
 j  +=  char_len ;
 
 res .len ++ ;
 
 }
 ;
 
return  res ;
 
 }
 int ustring_index_after(ustring u, ustring p, int start) {
 
 if ( p .len > u .len ) {
 
return  - 1 ;
 
 }
 ;
 
int strt= start ;
 
 if ( start < 0 ) {
 
 strt  =  0 ;
 
 }
 ;
 
 if ( start > u .len ) {
 
return  - 1 ;
 
 }
 ;
 
int i= strt ;
 
 while ( i < u .len ) {
 
 
int j= 0 ;
 
int ii= i ;
 
 while ( j < p .len  && string_eq( ustring_at ( u , ii ) , ustring_at ( p , j ) ) ) {
 
 
 j ++ ;
 
 ii ++ ;
 
 }
 ;
 
 if ( j == p .len ) {
 
return  i ;
 
 }
 ;
 
 i ++ ;
 
 }
 ;
 
return  - 1 ;
 
 }
 int ustring_count(ustring u, ustring substr) {
 
 if ( u .len == 0  ||  substr .len == 0 ) {
 
return  0 ;
 
 }
 ;
 
 if ( substr .len > u .len ) {
 
return  0 ;
 
 }
 ;
 
int n= 0 ;
 
int i= 0 ;
 
 while (1) { 
 
 i  =  ustring_index_after ( u , substr , i ) ;
 
 if ( i == - 1 ) {
 
return  n ;
 
 }
 ;
 
 i  +=  substr .len ;
 
 n ++ ;
 
 }
 ;
 
return  0 ;
 
 }
 string ustring_substr(ustring u, int _start, int _end) {
 
 if ( _start > _end  ||  _start > u .len  ||  _end > u .len  ||  _start < 0  ||  _end < 0 ) {
 
 v_panic ( _STR("substr(%d, %d) out of bounds (len=%d)", _start, _end, u .len) ) ;
 
 }
 ;
 
int end= ( _end >= u .len ) ? ( u .s .len ) : ( ( *(int*) array_get(  u .runes , _end) ) ) ;
 
return  string_substr ( u .s , ( *(int*) array_get(  u .runes , _start) ) , end ) ;
 
 }
 string ustring_left(ustring u, int pos) {
 
 if ( pos >= u .len ) {
 
return  u .s ;
 
 }
 ;
 
return  ustring_substr ( u , 0 , pos ) ;
 
 }
 string ustring_right(ustring u, int pos) {
 
 if ( pos >= u .len ) {
 
return  tos3("") ;
 
 }
 ;
 
return  ustring_substr ( u , pos , u .len ) ;
 
 }
 byte string_at(string s, int idx) {
 
 if ( idx < 0  ||  idx >= s .len ) {
 
 v_panic ( _STR("string index out of range: %d / %d", idx, s .len) ) ;
 
 }
 ;
 
return  s .str [/*ptr*/ idx ]/*rbyte 0*/ ;
 
 }
 string ustring_at(ustring u, int idx) {
 
 if ( idx < 0  ||  idx >= u .len ) {
 
 v_panic ( _STR("string index out of range: %d / %d", idx, u .runes .len) ) ;
 
 }
 ;
 
return  ustring_substr ( u , idx , idx + 1 ) ;
 
 }
 void v_ustring_free(ustring u) {
 
 v_array_free ( u .runes ) ;
 
 }
 bool byte_is_digit(byte c) {
 
return  c >= '0'  &&  c <= '9' ;
 
 }
 bool byte_is_hex_digit(byte c) {
 
return  byte_is_digit ( c )  ||  ( c >= 'a'  &&  c <= 'f' )  ||  ( c >= 'A'  &&  c <= 'F' ) ;
 
 }
 bool byte_is_oct_digit(byte c) {
 
return  c >= '0'  &&  c <= '7' ;
 
 }
 bool byte_is_letter(byte c) {
 
return  ( c >= 'a'  &&  c <= 'z' )  ||  ( c >= 'A'  &&  c <= 'Z' ) ;
 
 }
 void v_string_free(string s) {
 
 v_free ( s .str ) ;
 
 }
 string string_all_before(string s, string dot) {
 
int pos= string_index ( s , dot ) ;
 
 if ( pos == - 1 ) {
 
return  s ;
 
 }
 ;
 
return  string_left ( s , pos ) ;
 
 }
 string string_all_before_last(string s, string dot) {
 
int pos= string_last_index ( s , dot ) ;
 
 if ( pos == - 1 ) {
 
return  s ;
 
 }
 ;
 
return  string_left ( s , pos ) ;
 
 }
 string string_all_after(string s, string dot) {
 
int pos= string_last_index ( s , dot ) ;
 
 if ( pos == - 1 ) {
 
return  s ;
 
 }
 ;
 
return  string_right ( s , pos + dot .len ) ;
 
 }
 string array_string_join(array_string a, string del) {
 
 if ( a .len == 0 ) {
 
return  tos3("") ;
 
 }
 ;
 
int len= 0 ;
 
 array_string tmp30 =  a;
 for (int i = 0; i < tmp30.len; i++) {
 string val = ((string *) tmp30 . data)[i];
 
 
 len  +=  val .len + del .len ;
 
 }
 ;
 
 len  -=  del .len ;
 
string res= tos3("") ;
 
 res .len  =  len ;
 
 res .str  =  v_malloc ( res .len + 1 ) ;
 
int idx= 0 ;
 
 array_string tmp31 =  a;
 for (int i = 0; i < tmp31.len; i++) {
 string val = ((string *) tmp31 . data)[i];
 
 
 for (
int j= 0  ;  j < val .len  ;  j ++ ) { 
 
 
byte c= val .str[ j ]/*rbyte 0*/ ;
 
 res .str [/*ptr*/ idx ]/*rbyte 1*/  =  val .str [/*ptr*/ j ]/*rbyte 0*/ ;
 
 idx ++ ;
 
 }
 ;
 
 if ( i != a .len - 1 ) {
 
 for (
int k= 0  ;  k < del .len  ;  k ++ ) { 
 
 
 res .str [/*ptr*/ idx ]/*rbyte 1*/  =  del .str [/*ptr*/ k ]/*rbyte 0*/ ;
 
 idx ++ ;
 
 }
 ;
 
 }
 ;
 
 }
 ;
 
 res .str [/*ptr*/ res .len ]/*rbyte 1*/  =  '\0' ;
 
return  res ;
 
 }
 string array_string_join_lines(array_string s) {
 
return  array_string_join ( s , tos3("\n") ) ;
 
 }
 string string_reverse(string s) {
 
string res= (string) { .len =  s .len , .str =  v_malloc ( s .len ) } ;
 
 for (
int i= s .len - 1  ;  i >= 0  ;  i -- ) { 
 
 
 res .str[ s .len - i - 1 ]/*rbyte 1*/  =  s .str[ i ]/*rbyte 0*/ ;
 
 }
 ;
 
return  res ;
 
 }
 string string_limit(string s, int max) {
 
ustring u= string_ustring ( s ) ;
 
 if ( u .len <= max ) {
 
return  s ;
 
 }
 ;
 
return  ustring_substr ( u , 0 , max ) ;
 
 }
 bool byte_is_white(byte c) {
 
int i= ((int)( c ) ) ;
 
return  i == 10  ||  i == 32  ||  i == 9  ||  i == 13  ||  c == '\r' ;
 
 }
 int string_hash(string s) {
 
int h= 0 ;
 
 if ( h == 0  &&  s .len > 0 ) {
 
 string tmp32 =  s;
 array_byte bytes_tmp32 = string_bytes( tmp32 );
 ;
for (int tmp33 = 0; tmp33 < tmp32 .len; tmp33 ++) {
 byte c = ((byte *) bytes_tmp32 . data)[tmp33];
 
 
 h  =  h * 31 + ((int)( c ) ) ;
 
 }
 ;
 
 }
 ;
 
return  h ;
 
 }
 array_byte string_bytes(string s) {
 
 if ( s .len == 0 ) {
 
return new_array_from_c_array(0, 0, sizeof(byte), EMPTY_ARRAY_OF_ELEMS( byte, 0 ) {   TCCSKIP(0) }) ;
 
 }
 ;
 
array_byte buf= array_repeat (new_array_from_c_array(1, 1, sizeof(byte), EMPTY_ARRAY_OF_ELEMS( byte, 1 ) {  ((byte)( 0 ) )  }) , s .len ) ;
 
 memcpy ( buf .data , (char*) s .str ,  s .len ) ;
 
return  buf ;
 
 }
 string string_repeat(string s, int count) {
 
 if ( count <= 1 ) {
 
return  s ;
 
 }
 ;
 
byte* ret= v_malloc ( s .len * count + 1 ) ;
 
 int tmp34 =  0;
 ;
for (int tmp35 = tmp34; tmp35 <  count; tmp35++) {
 int i = tmp35;
 
 
 int tmp36 =  0;
 ;
for (int tmp37 = tmp36; tmp37 <  s .len; tmp37++) {
 int j = tmp37;
 
 
 ret [/*ptr*/ i * s .len + j ]/*rbyte 1*/  =  s .str[ j ]/*rbyte 0*/ ;
 
 }
 ;
 
 }
 ;
 
 ret [/*ptr*/ s .len * count ]/*rbyte 1*/  =  0 ;
 
return  (tos2((byte *) ret ) ) ;
 
 }
 int utf8_char_len(byte b) {
 
return  ( ( 0xe5000000  >>  ( ( b  >>  3 ) & 0x1e ) ) & 3 ) + 1 ;
 
 }
 string utf32_to_str(u32 code) {
 
int icode= ((int)( code ) ) ;
 
byte* buffer= v_malloc ( 5 ) ;
 
 if ( icode <= 127 ) {
 
 buffer [/*ptr*/ 0 ]/*rbyte 1*/  =  icode ;
 
return  tos ( buffer , 1 ) ;
 
 }
 ;
 
 if ( ( icode <= 2047 ) ) {
 
 buffer [/*ptr*/ 0 ]/*rbyte 1*/  =  192 | ( icode  >>  6 ) ;
 
 buffer [/*ptr*/ 1 ]/*rbyte 1*/  =  128 | ( icode & 63 ) ;
 
return  tos ( buffer , 2 ) ;
 
 }
 ;
 
 if ( ( icode <= 65535 ) ) {
 
 buffer [/*ptr*/ 0 ]/*rbyte 1*/  =  224 | ( icode  >>  12 ) ;
 
 buffer [/*ptr*/ 1 ]/*rbyte 1*/  =  128 | ( ( icode  >>  6 ) & 63 ) ;
 
 buffer [/*ptr*/ 2 ]/*rbyte 1*/  =  128 | ( icode & 63 ) ;
 
return  tos ( buffer , 3 ) ;
 
 }
 ;
 
 if ( ( icode <= 1114111 ) ) {
 
 buffer [/*ptr*/ 0 ]/*rbyte 1*/  =  240 | ( icode  >>  18 ) ;
 
 buffer [/*ptr*/ 1 ]/*rbyte 1*/  =  128 | ( ( icode  >>  12 ) & 63 ) ;
 
 buffer [/*ptr*/ 2 ]/*rbyte 1*/  =  128 | ( ( icode  >>  6 ) & 63 ) ;
 
 buffer [/*ptr*/ 3 ]/*rbyte 1*/  =  128 | ( icode & 63 ) ;
 
return  tos ( buffer , 4 ) ;
 
 }
 ;
 
return  tos3("") ;
 
 }
 string utf32_to_str_no_malloc(u32 code, void* buf) {
 
int icode= ((int)( code ) ) ;
 
byteptr buffer= ((byteptr)( buf ) ) ;
 
 if ( icode <= 127 ) {
 
 buffer [/*ptr*/ 0 ]/*rbyteptr 1*/  =  icode ;
 
return  tos ( buffer , 1 ) ;
 
 }
 ;
 
 if ( ( icode <= 2047 ) ) {
 
 buffer [/*ptr*/ 0 ]/*rbyteptr 1*/  =  192 | ( icode  >>  6 ) ;
 
 buffer [/*ptr*/ 1 ]/*rbyteptr 1*/  =  128 | ( icode & 63 ) ;
 
return  tos ( buffer , 2 ) ;
 
 }
 ;
 
 if ( ( icode <= 65535 ) ) {
 
 buffer [/*ptr*/ 0 ]/*rbyteptr 1*/  =  224 | ( icode  >>  12 ) ;
 
 buffer [/*ptr*/ 1 ]/*rbyteptr 1*/  =  128 | ( ( icode  >>  6 ) & 63 ) ;
 
 buffer [/*ptr*/ 2 ]/*rbyteptr 1*/  =  128 | ( icode & 63 ) ;
 
return  tos ( buffer , 3 ) ;
 
 }
 ;
 
 if ( ( icode <= 1114111 ) ) {
 
 buffer [/*ptr*/ 0 ]/*rbyteptr 1*/  =  240 | ( icode  >>  18 ) ;
 
 buffer [/*ptr*/ 1 ]/*rbyteptr 1*/  =  128 | ( ( icode  >>  12 ) & 63 ) ;
 
 buffer [/*ptr*/ 2 ]/*rbyteptr 1*/  =  128 | ( ( icode  >>  6 ) & 63 ) ;
 
 buffer [/*ptr*/ 3 ]/*rbyteptr 1*/  =  128 | ( icode & 63 ) ;
 
return  tos ( buffer , 4 ) ;
 
 }
 ;
 
return  tos3("") ;
 
 }
 int string_utf32_code(string _rune) {
 
 if ( _rune .len == 0 ) {
 
return  0 ;
 
 }
 ;
 
 if ( _rune .len == 1 ) {
 
return  ((int)( _rune .str[ 0 ]/*rbyte 0*/ ) ) ;
 
 }
 ;
 
byte b= ((byte)( ((int)( _rune .str[ 0 ]/*rbyte 0*/ ) ) ) ) ;
 
 b  =  b  <<  _rune .len ;
 
int res= ((int)( b ) ) ;
 
int shift= 6 - _rune .len ;
 
 for (
int i= 1  ;  i < _rune .len  ;  i ++ ) { 
 
 
int c= ((int)( _rune .str[ i ]/*rbyte 0*/ ) ) ;
 
 res  =  res  <<  shift ;
 
 res  |=  c & 63 ;
 
 shift  =  6 ;
 
 }
 ;
 
return  res ;
 
 }
 u16* string_to_wide(string _str) {
 
 #ifdef _WIN32
 
int num_chars= ((int)( MultiByteToWideChar ( builtin__CP_UTF8 ,  0 , (char*) _str .str ,  _str .len ,  0 ,  0 ) ) ) ;
 
u16* wstr= ((u16*)( v_malloc ( ( num_chars + 1 ) * 2 ) ) ) ;
 
 if ( wstr > 0 ) {
 
 MultiByteToWideChar ( builtin__CP_UTF8 ,  0 , (char*) _str .str ,  _str .len ,  wstr ,  num_chars ) ;
 
 memset ((char*) ((byte*)( wstr ) ) + num_chars * 2 ,  0 ,  2 ) ;
 
 }
 ;
 
return  wstr ;
 
 #else
 
return  0 ;
 
 #endif
 ;
 
 }
 string string_from_wide(u16* _wstr) {
 
 #ifdef _WIN32
 
int wstr_len= ((int)( wcslen ( _wstr ) ) ) ;
 
return  string_from_wide2 ( _wstr , wstr_len ) ;
 
 #else
 
return  tos3("") ;
 
 #endif
 ;
 
 }
 string string_from_wide2(u16* _wstr, int len) {
 
 #ifdef _WIN32
 
int num_chars= ((int)( WideCharToMultiByte ( builtin__CP_UTF8 ,  0 ,  _wstr ,  len ,  0 ,  0 ,  0 ,  0 ) ) ) ;
 
byte* str_to= ((byte*)( v_malloc ( num_chars + 1 ) ) ) ;
 
 if ( str_to > 0 ) {
 
 WideCharToMultiByte ( builtin__CP_UTF8 ,  0 ,  _wstr ,  len , (char*) str_to ,  num_chars ,  0 ,  0 ) ;
 
 memset ((char*) ((byte*)( str_to ) ) + num_chars ,  0 ,  1 ) ;
 
 }
 ;
 
return  tos2 ( str_to ) ;
 
 #else
 
return  tos3("") ;
 
 #endif
 ;
 
 }
 int utf8_len(byte c) {
 
int b= 0 ;
 
byte x= c ;
 
 if ( ( ( x & 240 ) != 0 ) ) {
 
 x  >>=  4 ;
 
 }
  else { 
 
 b  +=  4 ;
 
 }
 ;
 
 if ( ( ( x & 12 ) != 0 ) ) {
 
 x  >>=  2 ;
 
 }
  else { 
 
 b  +=  2 ;
 
 }
 ;
 
 if ( ( ( x & 2 ) == 0 ) ) {
 
 b ++ ;
 
 }
 ;
 
return  b ;
 
 }
 int utf8_getchar() {
 
int c= ((int)( getchar ( ) ) ) ;
 
int len= utf8_len ( ~ c ) ;
 
 if ( c < 0 ) {
 
return  0 ;
 
 }
  else  if ( len == 0 ) {
 
return  c ;
 
 }
  else  if ( len == 1 ) {
 
return  - 1 ;
 
 }
  else { 
 
int uc= ((int)( c & ( ( 1  <<  ( 7 - len ) ) - 1 ) ) ) ;
 
 for (
int i= 0  ;  i + 1 < len  ;  i ++ ) { 
 
 
int c2= ((int)( getchar ( ) ) ) ;
 
 if ( c2 != - 1  &&  ( c2  >>  6 ) == 2 ) {
 
 uc  <<=  6 ;
 
 uc  |=  ((int)( ( c2 & 63 ) ) ) ;
 
 }
  else  if ( c2 == - 1 ) {
 
return  0 ;
 
 }
  else { 
 
return  - 1 ;
 
 }
 ;
 
 }
 ;
 
return  uc ;
 
 }
 ;
 
 }
 int json__jsdecode_int(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  0 ;
 
 }
 ;
 
return  root ->valueint ;
 
 }
 i8 json__jsdecode_i8(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((i8)( 0 ) ) ;
 
 }
 ;
 
return  ((i8)( root ->valueint ) ) ;
 
 }
 i16 json__jsdecode_i16(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((i16)( 0 ) ) ;
 
 }
 ;
 
return  ((i16)( root ->valueint ) ) ;
 
 }
 i64 json__jsdecode_i64(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((i64)( 0 ) ) ;
 
 }
 ;
 
return  ((i64)( root ->valuedouble ) ) ;
 
 }
 byte json__jsdecode_byte(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((byte)( 0 ) ) ;
 
 }
 ;
 
return  ((byte)( root ->valueint ) ) ;
 
 }
 u16 json__jsdecode_u16(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((u16)( 0 ) ) ;
 
 }
 ;
 
return  ((u16)( root ->valueint ) ) ;
 
 }
 u32 json__jsdecode_u32(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((u32)( 0 ) ) ;
 
 }
 ;
 
return  ((u32)( root ->valueint ) ) ;
 
 }
 u64 json__jsdecode_u64(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((u64)( 0 ) ) ;
 
 }
 ;
 
return  ((u64)( root ->valueint ) ) ;
 
 }
 f32 json__jsdecode_f32(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((f32)( 0 ) ) ;
 
 }
 ;
 
return  ((f32)( root ->valuedouble ) ) ;
 
 }
 f64 json__jsdecode_f64(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  ((f64)( 0 ) ) ;
 
 }
 ;
 
return  ((f64)( root ->valuedouble ) ) ;
 
 }
 string json__jsdecode_string(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  tos3("") ;
 
 }
 ;
 
 if ( isnil ( root ->valuestring ) ) {
 
return  tos3("") ;
 
 }
 ;
 
return  tos_clone ( root ->valuestring ) ;
 
 }
 bool json__jsdecode_bool(cJSON* root) {
 
 if ( isnil ( root ) ) {
 
return  0 ;
 
 }
 ;
 
return  cJSON_IsTrue ( root ) ;
 
 }
 cJSON* json__jsencode_int(int val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_i8(i8 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_i16(i16 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_i64(i64 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_byte(byte val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_u16(u16 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_u32(u32 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_u64(u64 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_f32(f32 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_f64(f64 val) {
 
return  cJSON_CreateNumber ( val ) ;
 
 }
 cJSON* json__jsencode_bool(bool val) {
 
return  cJSON_CreateBool ( val ) ;
 
 }
 cJSON* json__jsencode_string(string val) {
 
string clone= string_clone ( val ) ;
 
return  cJSON_CreateString ((char*) clone .str ) ;
 
 }
 cJSON* json__json_parse(string s) {
 
return  cJSON_Parse ((char*) s .str ) ;
 
 }
 string json__json_print(cJSON* json) {
 
void* s= cJSON_PrintUnformatted ( json ) ;
 
return  tos ( s , strlen ( s ) ) ;
 
 }
 strings__Builder strings__new_builder(int initial_size) {
 
return  (strings__Builder) { .buf =  make ( 0 , initial_size , sizeof( byte) ) , .len =  0 } ;
 
 }
 void strings__Builder_write(strings__Builder* b, string s) {
 
 array_push_many (& /* ? */ b ->buf , s .str , s .len ) ;
 
 b ->len  +=  s .len ;
 
 }
 void strings__Builder_writeln(strings__Builder* b, string s) {
 
 array_push_many (& /* ? */ b ->buf , s .str , s .len ) ;
 
_PUSH(& b ->buf , ( /*typ = array_byte   tmp_typ=byte*/ '\n' ), tmp1, byte) ;
 
 b ->len  +=  s .len + 1 ;
 
 }
 string strings__Builder_str(strings__Builder b) {
 
return  (tos((byte *) b .buf .data ,  b .len ) ) ;
 
 }
 void strings__Builder_cut(strings__Builder* b, int n) {
 
 b ->len  -=  n ;
 
 }
 void strings__Builder_free(strings__Builder* b) {
 
 }
 int strings__levenshtein_distance(string a, string b) {
 
array_int f= array_repeat (new_array_from_c_array(1, 1, sizeof(int), EMPTY_ARRAY_OF_ELEMS( int, 1 ) {  0  }) , b .len + 1 ) ;
 
 string tmp1 =  a;
 array_byte bytes_tmp1 = string_bytes( tmp1 );
 ;
for (int tmp2 = 0; tmp2 < tmp1 .len; tmp2 ++) {
 byte ca = ((byte *) bytes_tmp1 . data)[tmp2];
 
 
int j= 1 ;
 
int fj1= ( *(int*) array_get(  f , 0) ) ;
 
 ( *(int*) array_get(  f , 0) ) ++ ;
 
 string tmp7 =  b;
 array_byte bytes_tmp7 = string_bytes( tmp7 );
 ;
for (int tmp8 = 0; tmp8 < tmp7 .len; tmp8 ++) {
 byte cb = ((byte *) bytes_tmp7 . data)[tmp8];
 
 
int mn= ( ( *(int*) array_get(  f , j) ) + 1 <= ( *(int*) array_get(  f , j - 1) ) + 1 ) ? ( ( *(int*) array_get(  f , j) ) + 1 ) : ( ( *(int*) array_get(  f , j - 1) ) + 1 ) ;
 
 if ( cb != ca ) {
 
 mn  =  ( mn <= fj1 + 1 ) ? ( mn ) : ( fj1 + 1 ) ;
 
 }
  else { 
 
 mn  =  ( mn <= fj1 ) ? ( mn ) : ( fj1 ) ;
 
 }
 ;
 
 fj1  =  ( *(int*) array_get(  f , j) ) ;
 
array_set(&/*q*/ f , j , & (int []) {  mn }) ;
 
 j ++ ;
 
 }
 ;
 
 }
 ;
 
return  ( *(int*) array_get(  f , f .len - 1) ) ;
 
 }
 f32 strings__levenshtein_distance_percentage(string a, string b) {
 
int d= strings__levenshtein_distance ( a , b ) ;
 
int l= ( a .len >= b .len ) ? ( a .len ) : ( b .len ) ;
 
return  ( 1.00 - ((f32)( d ) ) / ((f32)( l ) ) ) * 100.00 ;
 
 }
 f32 strings__dice_coefficient(string s1, string s2) {
 
 if ( s1 .len == 0  ||  s2 .len == 0 ) {
 
return  0.0 ;
 
 }
 ;
 
 if (string_eq( s1 , s2 ) ) {
 
return  1.0 ;
 
 }
 ;
 
 if ( s1 .len < 2  ||  s2 .len < 2 ) {
 
return  0.0 ;
 
 }
 ;
 
string a= ( s1 .len > s2 .len ) ? ( s1 ) : ( s2 ) ;
 
string b= (string_eq( a , s1 ) ) ? ( s2 ) : ( s1 ) ;
 
map_int first_bigrams= new_map(1, sizeof(int)) ;
 
 for (
int i= 0  ;  i < a .len - 1  ;  i ++ ) { 
 
 
string bigram= string_substr2( a , i , i + 2 , false) ;
  
 int tmp23 = 0; bool tmp24 = map_get(/*similarity.v : 47*/ first_bigrams , bigram, & tmp23); 

int q= (_IN_MAP( ( bigram ),  first_bigrams ) ) ? ( tmp23 + 1 ) : ( 1 ) ;
 
map_set(& first_bigrams , bigram , & (int []) {  q }) ;
 
 }
 ;
 
int intersection_size= 0 ;
 
 for (
int i= 0  ;  i < b .len - 1  ;  i ++ ) { 
 
 
string bigram= string_substr2( b , i , i + 2 , false) ;
  
 int tmp27 = 0; bool tmp28 = map_get(/*similarity.v : 53*/ first_bigrams , bigram, & tmp27); 

int count= (_IN_MAP( ( bigram ),  first_bigrams ) ) ? ( tmp27 ) : ( 0 ) ;
 
 if ( count > 0 ) {
 
map_set(& first_bigrams , bigram , & (int []) {  count - 1 }) ;
 
 intersection_size ++ ;
 
 }
 ;
 
 }
 ;
 
return  ( 2.0 * intersection_size ) / ( ((f32)( a .len ) ) + ((f32)( b .len ) ) - 2 ) ;
 
 }
 string strings__repeat(byte c, int n) {
 
 if ( n <= 0 ) {
 
return  tos3("") ;
 
 }
 ;
 
array_byte arr= array_repeat (new_array_from_c_array(1, 1, sizeof(byte), EMPTY_ARRAY_OF_ELEMS( byte, 1 ) {  c  }) , n + 1 ) ;
 
array_set(&/*q*/ arr , n , & (byte []) {  '\0' }) ;
 
return  (tos((byte *) arr .data ,  n ) ) ;
 
 }
 byte strconv__byte_to_lower(byte c) {
 
return  c | ( 'x' - 'X' ) ;
 
 }
 u64 strconv__parse_uint(string s, int _base, int _bit_size) {
 
int bit_size= _bit_size ;
 
int base= _base ;
 
 if ( s .len < 1  ||  ! strconv__underscore_ok ( s ) ) {
 
return  ((u64)( 0 ) ) ;
 
 }
 ;
 
bool base0= base == 0 ;
 
int start_index= 0 ;
 
 if ( 2 <= base  &&  base <= 36 ) {
 
 }
  else  if ( base == 0 ) {
 
 base  =  10 ;
 
 if ( string_at( s , 0) == '0' ) {
 
 if ( s .len >= 3  &&  strconv__byte_to_lower ( string_at( s , 1) ) == 'b' ) {
 
 base  =  2 ;
 
 start_index  +=  2 ;
 
 }
  else  if ( s .len >= 3  &&  strconv__byte_to_lower ( string_at( s , 1) ) == 'o' ) {
 
 base  =  8 ;
 
 start_index  +=  2 ;
 
 }
  else  if ( s .len >= 3  &&  strconv__byte_to_lower ( string_at( s , 1) ) == 'x' ) {
 
 base  =  16 ;
 
 start_index  +=  2 ;
 
 }
  else { 
 
 base  =  8 ;
 
 start_index ++ ;
 
 }
 ;
 
 }
 ;
 
 }
  else { 
 
return  ((u64)( 0 ) ) ;
 
 }
 ;
 
 if ( bit_size == 0 ) {
 
 bit_size  =  ((int)( strconv__int_size ) ) ;
 
 }
  else  if ( bit_size < 0  ||  bit_size > 64 ) {
 
return  ((u64)( 0 ) ) ;
 
 }
 ;
 
u64 cutoff= ((u64)( strconv__max_u64 / ((u64)( base ) ) ) ) + ((u64)( 1 ) ) ;
 
u64 max_val= ( bit_size == 64 ) ? ( strconv__max_u64 ) : ( ((u64)( ((u64)( 1 ) )  <<  ((u64)( bit_size ) ) ) ) - ((u64)( 1 ) ) ) ;
 
bool underscores= 0 ;
 
u64 n= ((u64)( 0 ) ) ;
 
 int tmp9 =  start_index;
 ;
for (int tmp10 = tmp9; tmp10 <  s .len; tmp10++) {
 int i = tmp10;
 
 
byte c= string_at( s , i) ;
 
byte cl= strconv__byte_to_lower ( c ) ;
 
byte d= ((byte)( 0 ) ) ;
 
 if ( c == '_'  &&  base0 ) {
 
 underscores  =  1 ;
 
 continue
 ;
 
 }
  else  if ( '0' <= c  &&  c <= '9' ) {
 
 d  =  c - '0' ;
 
 }
  else  if ( 'a' <= cl  &&  cl <= 'z' ) {
 
 d  =  cl - 'a' + 10 ;
 
 }
  else { 
 
return  ((u64)( 0 ) ) ;
 
 }
 ;
 
 if ( d >= ((byte)( base ) ) ) {
 
return  ((u64)( 0 ) ) ;
 
 }
 ;
 
 if ( n >= cutoff ) {
 
return  max_val ;
 
 }
 ;
 
 n  *=  ((u64)( base ) ) ;
 
u64 n1= n + ((u64)( d ) ) ;
 
 if ( n1 < n  ||  n1 > max_val ) {
 
return  max_val ;
 
 }
 ;
 
 n  =  n1 ;
 
 }
 ;
 
 if ( underscores  &&  ! strconv__underscore_ok ( s ) ) {
 
return  ((u64)( 0 ) ) ;
 
 }
 ;
 
return  n ;
 
 }
 i64 strconv__parse_int(string _s, int base, int _bit_size) {
 
string s= _s ;
 
int bit_size= _bit_size ;
 
 if ( s .len < 1 ) {
 
return  ((i64)( 0 ) ) ;
 
 }
 ;
 
bool neg= 0 ;
 
 if ( string_at( s , 0) == '+' ) {
 
 s  =  string_substr2( s , 1 , -1, true) ;
 
 }
  else  if ( string_at( s , 0) == '-' ) {
 
 neg  =  1 ;
 
 s  =  string_substr2( s , 1 , -1, true) ;
 
 }
 ;
 
u64 un= strconv__parse_uint ( s , base , bit_size ) ;
 
 if ( un == 0 ) {
 
return  ((i64)( 0 ) ) ;
 
 }
 ;
 
 if ( bit_size == 0 ) {
 
 bit_size  =  ((int)( strconv__int_size ) ) ;
 
 }
 ;
 
u64 cutoff= ((u64)( ((u64)( 1 ) )  <<  ((u64)( bit_size - 1 ) ) ) ) ;
 
 if ( ! neg  &&  un >= cutoff ) {
 
return  ((i64)( cutoff - ((u64)( 1 ) ) ) ) ;
 
 }
 ;
 
 if ( neg  &&  un > cutoff ) {
 
return  - ((i64)( cutoff ) ) ;
 
 }
 ;
 
return  ( neg ) ? ( - ((i64)( un ) ) ) : ( ((i64)( un ) ) ) ;
 
 }
 int strconv__atoi(string s) {
 
 if ( ( strconv__int_size == 32  &&  ( 0 < s .len  &&  s .len < 10 ) )  ||  ( strconv__int_size == 64  &&  ( 0 < s .len  &&  s .len < 19 ) ) ) {
 
int start_idx= 0 ;
 
 if ( string_at( s , 0) == '-'  ||  string_at( s , 0) == '+' ) {
 
 start_idx ++ ;
 
 if ( s .len - start_idx < 1 ) {
 
return  0 ;
 
 }
 ;
 
 }
 ;
 
int n= 0 ;
 
 int tmp25 =  start_idx;
 ;
for (int tmp26 = tmp25; tmp26 <  s .len; tmp26++) {
 int i = tmp26;
 
 
byte ch= string_at( s , i) - '0' ;
 
 if ( ch > 9 ) {
 
return  0 ;
 
 }
 ;
 
 n  =  n * 10 + ((int)( ch ) ) ;
 
 }
 ;
 
return  ( string_at( s , 0) == '-' ) ? ( - n ) : ( n ) ;
 
 }
 ;
 
i64 int64= strconv__parse_int ( s , 10 , 0 ) ;
 
return  ((int)( int64 ) ) ;
 
 }
 bool strconv__underscore_ok(string s) {
 
byte saw= '^' ;
 
int i= 0 ;
 
 if ( s .len >= 1  &&  ( string_at( s , 0) == '-'  ||  string_at( s , 0) == '+' ) ) {
 
 i ++ ;
 
 }
 ;
 
bool hex= 0 ;
 
 if ( s .len - i >= 2  &&  string_at( s , i) == '0'  &&  ( strconv__byte_to_lower ( string_at( s , i + 1) ) == 'b'  ||  strconv__byte_to_lower ( string_at( s , i + 1) ) == 'o'  ||  strconv__byte_to_lower ( string_at( s , i + 1) ) == 'x' ) ) {
 
 saw  =  '0' ;
 
 hex  =  strconv__byte_to_lower ( string_at( s , i + 1) ) == 'x' ;
 
 i  +=  2 ;
 
 }
 ;
 
 for (
  ;  i < s .len  ;  i ++ ) { 
 
 
 if ( ( '0' <= string_at( s , i)  &&  string_at( s , i) <= '9' )  ||  ( hex  &&  'a' <= strconv__byte_to_lower ( string_at( s , i) )  &&  strconv__byte_to_lower ( string_at( s , i) ) <= 'f' ) ) {
 
 saw  =  '0' ;
 
 continue
 ;
 
 }
 ;
 
 if ( string_at( s , i) == '_' ) {
 
 if ( saw != '0' ) {
 
return  0 ;
 
 }
 ;
 
 saw  =  '_' ;
 
 continue
 ;
 
 }
 ;
 
 if ( saw == '_' ) {
 
return  0 ;
 
 }
 ;
 
 saw  =  '!' ;
 
 }
 ;
 
return  saw != '_' ;
 
 }
 array_byte os__File_read_bytes(os__File f, int size) {
 
return  os__File_read_bytes_at ( f , size , 0 ) ;
 
 }
 array_byte os__File_read_bytes_at(os__File f, int size, int pos) {
 
byte* data= v_malloc ( size ) ;
 
array_byte arr= array_repeat (new_array_from_c_array(1, 1, sizeof(byte), EMPTY_ARRAY_OF_ELEMS( byte, 1 ) {  '0'  }) , size ) ;
 
 fseek ( f .cfile ,  pos ,  SEEK_SET ) ;
 
 fread ((char*) data ,  1 ,  size ,  f .cfile ) ;
 
 fseek ( f .cfile ,  0 ,  SEEK_SET ) ;
 
 for (
int e= 0  ;  e < size  ;  e ++ ) { 
 
 
array_set(&/*q*/ arr , e , & (byte []) {  data [/*ptr*/ e ]/*rbyte 1*/ }) ;
 
 }
 ;
 
return  arr ;
 
 }
 Option_string os__read_file(string path) {
 
string mode= tos3("rb") ;
 
FILE* fp= os__vfopen ( path , mode ) ;
 
 if ( isnil ( fp ) ) {
 
return  v_error ( _STR("failed to open file \"%.*s\"", path.len, path.str) ) ;
 
 }
 ;
 
 fseek ( fp ,  0 ,  SEEK_END ) ;
 
int fsize= ftell ( fp ) ;
 
 rewind ( fp ) ;
 
byte* str= v_malloc ( fsize + 1 ) ;
 
 fread ((char*) str ,  fsize ,  1 ,  fp ) ;
 
 fclose ( fp ) ;
 
 str [/*ptr*/ fsize ]/*rbyte 1*/  =  0 ;
 
string tmp1 = OPTION_CAST(string)( (tos((byte *) str ,  fsize ) )); 
 return opt_ok(&tmp1, sizeof(string)) ;
 
 }
 int os__file_size(string path) {
  
 struct /*c struct init*/ 

stat s ;
 
 #ifdef _WIN32
 
 _wstat ( string_to_wide ( path ) ,  & s ) ;
 
 #else
 
 stat ( ((char*)( path .str ) ) ,  & s ) ;
 
 #endif
 ;
 
return  s .st_size ;
 
 }
 void os__mv(string old, string new) {
 
 #ifdef _WIN32
 
 _wrename ( string_to_wide ( old ) ,  string_to_wide ( new ) ) ;
 
 #else
 
 rename ( ((char*)( old .str ) ) ,  ((char*)( new .str ) ) ) ;
 
 #endif
 ;
 
 }
 void os__cp(string old, string new) {
 
 #ifdef _WIN32
 
 v_panic ( tos3("not implemented") ) ;
 
 #else
 
 os__system ( _STR("cp %.*s %.*s", old.len, old.str, new.len, new.str) ) ;
 
 #endif
 ;
 
 }
 FILE* os__vfopen(string path, string mode) {
 
 #ifdef _WIN32
 
return  _wfopen ( string_to_wide ( path ) ,  string_to_wide ( mode ) ) ;
 
 #else
 
return  fopen ( ((char*)( path .str ) ) ,  ((char*)( mode .str ) ) ) ;
 
 #endif
 ;
 
 }
 array_string os__read_lines(string path) {
 
array_string res=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
int buf_len= 1024 ;
 
byte* buf= v_malloc ( buf_len ) ;
 
string mode= tos3("rb") ;
 
FILE* fp= os__vfopen ( path , mode ) ;
 
 if ( isnil ( fp ) ) {
 
return  res ;
 
 }
 ;
 
int buf_index= 0 ;
 
 while ( fgets ((char*) buf + buf_index ,  buf_len - buf_index ,  fp ) != 0 ) {
 
 
int len= vstrlen ( buf ) ;
 
 if ( len == buf_len - 1  &&  buf [/*ptr*/ len - 1 ]/*rbyte 1*/ != 10 ) {
 
 buf_len  *=  2 ;
 
 buf  =  realloc ((char*) buf ,  buf_len ) ;
 
 if ( isnil ( buf ) ) {
 
 v_panic ( tos3("Could not reallocate the read buffer") ) ;
 
 }
 ;
 
 buf_index  =  len ;
 
 continue
 ;
 
 }
 ;
 
 if ( buf [/*ptr*/ len - 1 ]/*rbyte 1*/ == 10  ||  buf [/*ptr*/ len - 1 ]/*rbyte 1*/ == 13 ) {
 
 buf [/*ptr*/ len - 1 ]/*rbyte 1*/  =  '\0' ;
 
 }
 ;
 
 if ( len > 1  &&  buf [/*ptr*/ len - 2 ]/*rbyte 1*/ == 13 ) {
 
 buf [/*ptr*/ len - 2 ]/*rbyte 1*/  =  '\0' ;
 
 }
 ;
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ tos_clone ( buf ) ), tmp2, string) ;
 
 buf_index  =  0 ;
 
 }
 ;
 
 fclose ( fp ) ;
 
return  res ;
 
 }
 array_ustring os__read_ulines(string path) {
 
array_string lines= os__read_lines ( path ) ;
 
array_ustring ulines=new_array_from_c_array(0, 0, sizeof(ustring), EMPTY_ARRAY_OF_ELEMS( ustring, 0 ) {   TCCSKIP(0) }) ;
 
 array_string tmp3 =  lines;
 for (int tmp4 = 0; tmp4 < tmp3.len; tmp4++) {
 string myline = ((string *) tmp3 . data)[tmp4];
 
 
_PUSH(& ulines , ( /*typ = array_ustring   tmp_typ=ustring*/ string_ustring ( myline ) ), tmp5, ustring) ;
 
 }
 ;
 
return  ulines ;
 
 }
 Option_os__File os__open(string path) {
 
os__File file= (os__File) { .cfile =  0 } ;
 
 #ifdef _WIN32
 
u16* wpath= string_to_wide ( path ) ;
 
string mode= tos3("rb") ;
 
 file  =  (os__File) { .cfile =  _wfopen ( wpath ,  string_to_wide ( mode ) ) } ;
 
 #else
 
byte* cpath= path .str ;
 
 file  =  (os__File) { .cfile =  fopen ( ((char*)( cpath ) ) ,  "rb" ) } ;
 
 #endif
 ;
 
 if ( isnil ( file .cfile ) ) {
 
return  v_error ( _STR("failed to open file \"%.*s\"", path.len, path.str) ) ;
 
 }
 ;
 
os__File tmp6 = OPTION_CAST(os__File)( file); 
 return opt_ok(&tmp6, sizeof(os__File)) ;
 
 }
 Option_os__File os__create(string path) {
 
os__File file= (os__File) { .cfile =  0 } ;
 
 #ifdef _WIN32
 
u16* wpath= string_to_wide ( string_replace ( path , tos3("/") , tos3("\\") ) ) ;
 
string mode= tos3("wb") ;
 
 file  =  (os__File) { .cfile =  _wfopen ( wpath ,  string_to_wide ( mode ) ) } ;
 
 #else
 
byte* cpath= path .str ;
 
 file  =  (os__File) { .cfile =  fopen ( ((char*)( cpath ) ) ,  "wb" ) } ;
 
 #endif
 ;
 
 if ( isnil ( file .cfile ) ) {
 
return  v_error ( _STR("failed to create file \"%.*s\"", path.len, path.str) ) ;
 
 }
 ;
 
os__File tmp7 = OPTION_CAST(os__File)( file); 
 return opt_ok(&tmp7, sizeof(os__File)) ;
 
 }
 Option_os__File os__open_append(string path) {
 
os__File file= (os__File) { .cfile =  0 } ;
 
 #ifdef _WIN32
 
u16* wpath= string_to_wide ( string_replace ( path , tos3("/") , tos3("\\") ) ) ;
 
string mode= tos3("ab") ;
 
 file  =  (os__File) { .cfile =  _wfopen ( wpath ,  string_to_wide ( mode ) ) } ;
 
 #else
 
byte* cpath= path .str ;
 
 file  =  (os__File) { .cfile =  fopen ( ((char*)( cpath ) ) ,  "ab" ) } ;
 
 #endif
 ;
 
 if ( isnil ( file .cfile ) ) {
 
return  v_error ( _STR("failed to create(append) file \"%.*s\"", path.len, path.str) ) ;
 
 }
 ;
 
os__File tmp8 = OPTION_CAST(os__File)( file); 
 return opt_ok(&tmp8, sizeof(os__File)) ;
 
 }
 void os__File_write(os__File f, string s) {
 
 fputs ((char*) s .str ,  f .cfile ) ;
 
 }
 void os__File_write_bytes(os__File f, void* data, int size) {
 
 fwrite ( data ,  1 ,  size ,  f .cfile ) ;
 
 }
 void os__File_write_bytes_at(os__File f, void* data, int size, int pos) {
 
 fseek ( f .cfile ,  pos ,  SEEK_SET ) ;
 
 fwrite ( data ,  1 ,  size ,  f .cfile ) ;
 
 fseek ( f .cfile ,  0 ,  SEEK_END ) ;
 
 }
 void os__File_writeln(os__File f, string s) {
 
 fputs ((char*) s .str ,  f .cfile ) ;
 
 fputs ( "\n" ,  f .cfile ) ;
 
 }
 void os__File_flush(os__File f) {
 
 fflush ( f .cfile ) ;
 
 }
 void os__File_close(os__File f) {
 
 fclose ( f .cfile ) ;
 
 }
 FILE* os__vpopen(string path) {
 
 #ifdef _WIN32
 
string mode= tos3("rb") ;
 
u16* wpath= string_to_wide ( path ) ;
 
return  _wpopen ( wpath ,  string_to_wide ( mode ) ) ;
 
 #else
 
byte* cpath= path .str ;
 
return  popen ((char*) cpath ,  "r" ) ;
 
 #endif
 ;
 
 }
 _V_MulRet_int_V_bool os__posix_wait4_to_exit_status(int waitret) {
 
 #ifdef _WIN32
 
return (_V_MulRet_int_V_bool){.var_0=waitret,.var_1=0} ;
 
 #else
 
int ret= 0 ;
 
bool is_signaled= 1 ;
 
 if ( WIFEXITED ( waitret ) ) {
 
 ret  =  WEXITSTATUS ( waitret ) ;
 
 is_signaled  =  0 ;
 
 }
  else  if ( WIFSIGNALED ( waitret ) ) {
 
 ret  =  WTERMSIG ( waitret ) ;
 
 is_signaled  =  1 ;
 
 }
 ;
 
return (_V_MulRet_int_V_bool){.var_0=ret,.var_1=is_signaled} ;
 
 #endif
 ;
 
 }
 int os__vpclose(FILE* f) {
 
 #ifdef _WIN32
 
return  ((int)( _pclose ( f ) ) ) ;
 
 #else
 
_V_MulRet_int_V_bool _V_mret_ret__= os__posix_wait4_to_exit_status ( ((int)( pclose ( f ) ) ) ) ;
 int ret = _V_mret_ret__.var_0 ;
 
return  ret ;
 
 #endif
 ;
 
 }
 Option_os__Result os__exec(string cmd) {
 
string pcmd= _STR("%.*s 2>&1", cmd.len, cmd.str) ;
 
FILE* f= os__vpopen ( pcmd ) ;
 
 if ( isnil ( f ) ) {
 
return  v_error ( _STR("exec(\"%.*s\") failed", cmd.len, cmd.str) ) ;
 
 }
 ;
 
byte buf  [1000 ] = { 0 } ;
 
string res= tos3("") ;
 
 while ( fgets ( ((char*)( buf ) ) ,  1000 ,  f ) != 0 ) {
 
 
 res = string_add(res,  tos ( buf , vstrlen ( buf ) ) ) ;
 
 }
 ;
 
 res  =  string_trim_space ( res ) ;
 
int exit_code= os__vpclose ( f ) ;
 
os__Result tmp9 = OPTION_CAST(os__Result)( (os__Result) { .output =  res , .exit_code =  exit_code }); 
 return opt_ok(&tmp9, sizeof(os__Result)) ;
 
 }
 int os__system(string cmd) {
 
int ret= ((int)( 0 ) ) ;
 
 #ifdef _WIN32
 
 ret  =  _wsystem ( string_to_wide ( cmd ) ) ;
 
 #else
 
 ret  =  system ((char*) cmd .str ) ;
 
 #endif
 ;
 
 if ( ret == - 1 ) {
 
 os__print_c_errno ( ) ;
 
 }
 ;
 
 #ifndef _WIN32
 
_V_MulRet_int_V_bool _V_mret_pret_is_signaled= os__posix_wait4_to_exit_status ( ret ) ;
 int pret = _V_mret_pret_is_signaled.var_0 ;
 bool is_signaled = _V_mret_pret_is_signaled.var_1 ;
 
 if ( is_signaled ) {
 
 println (string_add(string_add( _STR("Terminated by signal %2d (", ret) , os__sigint_to_signal_name ( pret ) ) , tos3(")") ) ) ;
 
 }
 ;
 
 ret  =  pret ;
 
 #endif
 ;
 
return  ret ;
 
 }
 string os__sigint_to_signal_name(int si) {
  
 int tmp10 =  si; 

 if ( tmp10 ==  1 ) { 
 
return  tos3("SIGHUP") ;
 
 }
 else  if ( tmp10 ==  2 ) { 
 
return  tos3("SIGINT") ;
 
 }
 else  if ( tmp10 ==  3 ) { 
 
return  tos3("SIGQUIT") ;
 
 }
 else  if ( tmp10 ==  4 ) { 
 
return  tos3("SIGILL") ;
 
 }
 else  if ( tmp10 ==  6 ) { 
 
return  tos3("SIGABRT") ;
 
 }
 else  if ( tmp10 ==  8 ) { 
 
return  tos3("SIGFPE") ;
 
 }
 else  if ( tmp10 ==  9 ) { 
 
return  tos3("SIGKILL") ;
 
 }
 else  if ( tmp10 ==  11 ) { 
 
return  tos3("SIGSEGV") ;
 
 }
 else  if ( tmp10 ==  13 ) { 
 
return  tos3("SIGPIPE") ;
 
 }
 else  if ( tmp10 ==  14 ) { 
 
return  tos3("SIGALRM") ;
 
 }
 else  if ( tmp10 ==  15 ) { 
 
return  tos3("SIGTERM") ;
 
 }
 ;
 
 #ifdef __linux__
  
 int tmp11 =  si; 

 if (( tmp11 ==  30 ) || ( tmp11 ==  10 ) || ( tmp11 ==  16 )  ) { 
 
return  tos3("SIGUSR1") ;
 
 }
 else  if (( tmp11 ==  31 ) || ( tmp11 ==  12 ) || ( tmp11 ==  17 )  ) { 
 
return  tos3("SIGUSR2") ;
 
 }
 else  if (( tmp11 ==  20 ) || ( tmp11 ==  17 ) || ( tmp11 ==  18 )  ) { 
 
return  tos3("SIGCHLD") ;
 
 }
 else  if (( tmp11 ==  19 ) || ( tmp11 ==  18 ) || ( tmp11 ==  25 )  ) { 
 
return  tos3("SIGCONT") ;
 
 }
 else  if (( tmp11 ==  17 ) || ( tmp11 ==  19 ) || ( tmp11 ==  23 )  ) { 
 
return  tos3("SIGSTOP") ;
 
 }
 else  if (( tmp11 ==  18 ) || ( tmp11 ==  20 ) || ( tmp11 ==  24 )  ) { 
 
return  tos3("SIGTSTP") ;
 
 }
 else  if (( tmp11 ==  21 ) || ( tmp11 ==  21 ) || ( tmp11 ==  26 )  ) { 
 
return  tos3("SIGTTIN") ;
 
 }
 else  if (( tmp11 ==  22 ) || ( tmp11 ==  22 ) || ( tmp11 ==  27 )  ) { 
 
return  tos3("SIGTTOU") ;
 
 }
 else  if ( tmp11 ==  5 ) { 
 
return  tos3("SIGTRAP") ;
 
 }
 else  if ( tmp11 ==  7 ) { 
 
return  tos3("SIGBUS") ;
 
 }
 ;
 
 #endif
 ;
 
return  tos3("unknown") ;
 
 }
 string os__getenv(string key) {
 
 #ifdef _WIN32
 
void* s= _wgetenv ( string_to_wide ( key ) ) ;
 
 if ( isnil ( s ) ) {
 
return  tos3("") ;
 
 }
 ;
 
return  string_from_wide ( s ) ;
 
 #else
 
byte* s= ((byte*)( getenv ((char*) key .str ) ) ) ;
 
 if ( isnil ( s ) ) {
 
return  tos3("") ;
 
 }
 ;
 
return  (tos2((byte *) s ) ) ;
 
 #endif
 ;
 
 }
 int os__setenv(string name, string value, bool overwrite) {
 
 #ifdef _WIN32
 
string format= _STR("%.*s=%.*s", name.len, name.str, value.len, value.str) ;
 
 if ( overwrite ) {
 
return  _putenv ((char*) format .str ) ;
 
 }
 ;
 
return  - 1 ;
 
 #else
 
return  setenv ((char*) name .str , (char*) value .str ,  overwrite ) ;
 
 #endif
 ;
 
 }
 int os__unsetenv(string name) {
 
 #ifdef _WIN32
 
string format= _STR("%.*s=", name.len, name.str) ;
 
return  _putenv ((char*) format .str ) ;
 
 #else
 
return  unsetenv ((char*) name .str ) ;
 
 #endif
 ;
 
 }
 bool os__file_exists(string _path) {
 
 #ifdef _WIN32
 
string path= string_replace ( _path , tos3("/") , tos3("\\") ) ;
 
return  _waccess ( string_to_wide ( path ) ,  0 ) != - 1 ;
 
 #else
 
return  access ((char*) _path .str ,  0 ) != - 1 ;
 
 #endif
 ;
 
 }
 void os__rm(string path) {
 
 #ifdef _WIN32
 
 _wremove ( string_to_wide ( path ) ) ;
 
 #else
 
 remove ((char*) path .str ) ;
 
 #endif
 ;
 
 }
 void os__rmdir(string path) {
 
 #ifndef _WIN32
 
 rmdir ((char*) path .str ) ;
 
 #else
 
 RemoveDirectory ( string_to_wide ( path ) ) ;
 
 #endif
 ;
 
 }
 void os__print_c_errno() {
 
 }
 string os__ext(string path) {
 
int pos= string_last_index ( path , tos3(".") ) ;
 
 if ( pos == - 1 ) {
 
return  tos3("") ;
 
 }
 ;
 
return  string_substr2( path , pos , -1, true) ;
 
 }
 string os__dir(string path) {
 
 if (string_eq( path , tos3(".") ) ) {
 
return  os__getwd ( ) ;
 
 }
 ;
 
int pos= string_last_index ( path , os__path_separator ) ;
 
 if ( pos == - 1 ) {
 
return  tos3(".") ;
 
 }
 ;
 
return  string_substr2( path , 0 , pos , false) ;
 
 }
 string os__path_sans_ext(string path) {
 
int pos= string_last_index ( path , tos3(".") ) ;
 
 if ( pos == - 1 ) {
 
return  path ;
 
 }
 ;
 
return  string_substr2( path , 0 , pos , false) ;
 
 }
 string os__basedir(string path) {
 
int pos= string_last_index ( path , os__path_separator ) ;
 
 if ( pos == - 1 ) {
 
return  path ;
 
 }
 ;
 
return  string_substr2( path , 0 , pos + 1 , false) ;
 
 }
 string os__filename(string path) {
 
return  string_all_after ( path , os__path_separator ) ;
 
 }
 string os__get_line() {
 
string str= os__get_raw_line ( ) ;
 
 #ifdef _WIN32
 
return  string_trim_right ( str , tos3("\r\n") ) ;
 
 #else
 
return  string_trim_right ( str , tos3("\n") ) ;
 
 #endif
 ;
 
 }
 string os__get_raw_line() {
 
 #ifdef _WIN32
 
int max_line_chars= 256 ;
 
byte* buf= ((byte*)( v_malloc ( max_line_chars * 2 ) ) ) ;
 
 if ( is_atty ( 0 ) ) {
 
void* h_input= GetStdHandle ( os__STD_INPUT_HANDLE ) ;
 
int nr_chars= 0 ;
 
 ReadConsole ( h_input , (char*) buf ,  max_line_chars * 2 ,  & nr_chars ,  0 ) ;
 
return  string_from_wide2 ( ((u16*)( buf ) ) , nr_chars ) ;
 
 }
 ;
 
int res= ((int)( fgetws ((char*) buf ,  max_line_chars ,  stdin ) ) ) ;
 
int len= ((int)( wcslen ( ((u16*)( buf ) ) ) ) ) ;
 
 if ( 0 != res ) {
 
return  string_from_wide2 ( ((u16*)( buf ) ) , len ) ;
 
 }
 ;
 
return  tos3("") ;
 
 #else
 
size_t max= ((size_t)( 256 ) ) ;
 
char* buf= ((char*)( v_malloc ( ((int)( max ) ) ) ) ) ;
 
int nr_chars= getline ( & buf ,  & max ,  stdin ) ;
 
 if ( nr_chars == 0 ) {
 
return  tos3("") ;
 
 }
 ;
 
return  (tos((byte *) ((byteptr)( buf ) ) ,  nr_chars ) ) ;
 
 #endif
 ;
 
 }
 array_string os__get_lines() {
 
string line= tos3("") ;
 
array_string inputstr=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 while (1) { 
 
 line  =  os__get_line ( ) ;
 
 if ( ( line .len <= 0 ) ) {
 
 break
 ;
 
 }
 ;
 
 line  =  string_trim_space ( line ) ;
 
_PUSH(& inputstr , ( /*typ = array_string   tmp_typ=string*/ line ), tmp20, string) ;
 
 }
 ;
 
return  inputstr ;
 
 }
 string os__get_lines_joined() {
 
string line= tos3("") ;
 
string inputstr= tos3("") ;
 
 while (1) { 
 
 line  =  os__get_line ( ) ;
 
 if ( line .len <= 0 ) {
 
 break
 ;
 
 }
 ;
 
 line  =  string_trim_space ( line ) ;
 
 inputstr = string_add(inputstr,  line ) ;
 
 }
 ;
 
return  inputstr ;
 
 }
 string os__user_os() {
 
 #ifdef __linux__
 
return  tos3("linux") ;
 
 #endif
 ;
 
 #ifdef __APPLE__
 
return  tos3("mac") ;
 
 #endif
 ;
 
 #ifdef _WIN32
 
return  tos3("windows") ;
 
 #endif
 ;
 
 #ifdef __FreeBSD__
 
return  tos3("freebsd") ;
 
 #endif
 ;
 
 #ifdef __OpenBSD__
 
return  tos3("openbsd") ;
 
 #endif
 ;
 
 #ifdef __NetBSD__
 
return  tos3("netbsd") ;
 
 #endif
 ;
 
 #ifdef __DragonFly__
 
return  tos3("dragonfly") ;
 
 #endif
 ;
 
 #ifdef __BIONIC__
 
return  tos3("android") ;
 
 #endif
 ;
 
 #ifdef __sun
 
return  tos3("solaris") ;
 
 #endif
 ;
 
return  tos3("unknown") ;
 
 }
 string os__home_dir() {
 
string home= os__getenv ( tos3("HOME") ) ;
 
 #ifdef _WIN32
 
 home  =  os__getenv ( tos3("HOMEDRIVE") ) ;
 
 if ( home .len == 0 ) {
 
 home  =  os__getenv ( tos3("SYSTEMDRIVE") ) ;
 
 }
 ;
 
string homepath= os__getenv ( tos3("HOMEPATH") ) ;
 
 if ( homepath .len == 0 ) {
 
 homepath  = string_add( tos3("\\Users\\") , os__getenv ( tos3("USERNAME") ) ) ;
 
 }
 ;
 
 home = string_add(home,  homepath ) ;
 
 #endif
 ;
 
 home = string_add(home,  os__path_separator ) ;
 
return  home ;
 
 }
 void os__write_file(string path, string text) {
 
Option_os__File tmp21 =  os__create ( path ) ;
 if (!tmp21 .ok) {
 string err = tmp21 . error;
 int errcode = tmp21 . ecode;
 
 
 return ;
 
 }
 os__File f = *(os__File*) tmp21 . data;
 ;
 
 os__File_write ( f , text ) ;
 
 os__File_close ( f ) ;
 
 }
 void os__clear() {
 
 #ifndef _WIN32
 
 printf ( "\x1b[2J" ) ;
 
 printf ( "\x1b[H" ) ;
 
 #endif
 ;
 
 }
 void os__on_segfault(void* f) {
 
 #ifdef _WIN32
 
 
 return ;
 
 #endif
 ;
 
 #ifdef __APPLE__
  
 struct /*c struct init*/ 

sigaction sa ;
 
 memset ( & sa ,  0 ,  sizeof( sigaction) ) ;
 
 sigemptyset ( & sa .sa_mask ) ;
 
 sa .sa_sigaction  =  f ;
 
 sa .sa_flags  =  SA_SIGINFO ;
 
 sigaction ( SIGSEGV ,  & sa ,  0 ) ;
 
 #endif
 ;
 
 }
 string os__executable() {
 
 #ifdef __linux__
 
byte* result= v_malloc ( os__MAX_PATH ) ;
 
int count= ((int)( readlink ( "/proc/self/exe" , (char*) result ,  os__MAX_PATH ) ) ) ;
 
 if ( count < 0 ) {
 
 v_panic ( tos3("error reading /proc/self/exe to get exe path") ) ;
 
 }
 ;
 
return  (tos((byte *) result ,  count ) ) ;
 
 #endif
 ;
 
 #ifdef _WIN32
 
int max= 512 ;
 
u16* result= ((u16*)( v_malloc ( max * 2 ) ) ) ;
 
int len= ((int)( GetModuleFileName ( 0 ,  result ,  max ) ) ) ;
 
return  string_from_wide2 ( result , len ) ;
 
 #endif
 ;
 
 #ifdef __APPLE__
 
byte* result= v_malloc ( os__MAX_PATH ) ;
 
int pid= getpid ( ) ;
 
int ret= proc_pidpath ( pid , (char*) result ,  os__MAX_PATH ) ;
 
 if ( ret <= 0 ) {
 
 println ( tos3("os.executable() failed") ) ;
 
return  tos3(".") ;
 
 }
 ;
 
return  (tos2((byte *) result ) ) ;
 
 #endif
 ;
 
 #ifdef __FreeBSD__
 
byte* result= v_malloc ( os__MAX_PATH ) ;
 
array_int mib=new_array_from_c_array(4, 4, sizeof(int), EMPTY_ARRAY_OF_ELEMS( int, 4 ) {  1 ,  14 ,  12 ,  - 1  }) ;
 
int size= os__MAX_PATH ;
 
 sysctl ( mib .data ,  4 , (char*) result ,  & size ,  0 ,  0 ) ;
 
return  (tos2((byte *) result ) ) ;
 
 #endif
 ;
 
 #ifdef __OpenBSD__
 
return  ( *(string*) array_get(  os__args , 0) ) ;
 
 #endif
 ;
 
 #ifdef __sun
 
 #endif
 ;
 
 #ifdef __NetBSD__
 
byte* result= v_malloc ( os__MAX_PATH ) ;
 
int count= ((int)( readlink ( "/proc/curproc/exe" , (char*) result ,  os__MAX_PATH ) ) ) ;
 
 if ( count < 0 ) {
 
 v_panic ( tos3("error reading /proc/curproc/exe to get exe path") ) ;
 
 }
 ;
 
return  (tos((byte *) result ,  count ) ) ;
 
 #endif
 ;
 
 #ifdef __DragonFly__
 
byte* result= v_malloc ( os__MAX_PATH ) ;
 
int count= ((int)( readlink ( "/proc/curproc/file" , (char*) result ,  os__MAX_PATH ) ) ) ;
 
 if ( count < 0 ) {
 
 v_panic ( tos3("error reading /proc/curproc/file to get exe path") ) ;
 
 }
 ;
 
return  (tos((byte *) result ,  count ) ) ;
 
 #endif
 ;
 
return  ( *(string*) array_get(  os__args , 0) ) ;
 
 }
 bool os__is_dir(string path) {
 
 #ifdef _WIN32
 
return  os__dir_exists ( path ) ;
 
 #else
  
 struct /*c struct init*/ 

stat statbuf ;
 
byte* cstr= path .str ;
 
 if ( stat ((char*) cstr ,  & statbuf ) != 0 ) {
 
return  0 ;
 
 }
 ;
 
return  ( statbuf .st_mode & os__S_IFMT ) == os__S_IFDIR ;
 
 #endif
 ;
 
 }
 void os__chdir(string path) {
 
 #ifdef _WIN32
 
 _wchdir ( string_to_wide ( path ) ) ;
 
 #else
 
 chdir ((char*) path .str ) ;
 
 #endif
 ;
 
 }
 string os__getwd() {
 
 #ifdef _WIN32
 
int max= 512 ;
 
u16* buf= ((u16*)( v_malloc ( max * 2 ) ) ) ;
 
 if ( _wgetcwd ( buf ,  max ) == 0 ) {
 
return  tos3("") ;
 
 }
 ;
 
return  string_from_wide ( buf ) ;
 
 #else
 
byte* buf= v_malloc ( 512 ) ;
 
 if ( getcwd ((char*) buf ,  512 ) == 0 ) {
 
return  tos3("") ;
 
 }
 ;
 
return  (tos2((byte *) buf ) ) ;
 
 #endif
 ;
 
 }
 string os__realpath(string fpath) {
 
byte* fullpath= v_malloc ( os__MAX_PATH ) ;
 
int res= 0 ;
 
 #ifdef _WIN32
 
 res  =  ((int)( _fullpath ((char*) fullpath , (char*) fpath .str ,  os__MAX_PATH ) ) ) ;
 
 #else
 
 res  =  ((int)( realpath ((char*) fpath .str , (char*) fullpath ) ) ) ;
 
 #endif
 ;
 
 if ( res != 0 ) {
 
return  (tos((byte *) fullpath ,  vstrlen ( fullpath ) ) ) ;
 
 }
 ;
 
return  fpath ;
 
 }
 array_string os__walk_ext(string path, string ext) {
 
 if ( ! os__is_dir ( path ) ) {
 
return new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 }
 ;
 
Option_array_string tmp26 =  os__ls ( path ) ;
 if (!tmp26 .ok) {
 string err = tmp26 . error;
 int errcode = tmp26 . ecode;
 
 v_panic ( err ) ;
 
 }
 array_string files = *(array_string*) tmp26 . data;
 ;
 
array_string res=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 array_string tmp27 =  files;
 for (int i = 0; i < tmp27.len; i++) {
 string file = ((string *) tmp27 . data)[i];
 
 
 if ( string_starts_with ( file , tos3(".") ) ) {
 
 continue
 ;
 
 }
 ;
 
string p=string_add(string_add( path , os__path_separator ) , file ) ;
 
 if ( os__is_dir ( p ) ) {
 
_PUSH_MANY(& res , ( /*typ = array_string   tmp_typ=string*/ os__walk_ext ( p , ext ) ), tmp28, array_string) ;
 
 }
  else  if ( string_ends_with ( file , ext ) ) {
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ p ), tmp29, string) ;
 
 }
 ;
 
 }
 ;
 
return  res ;
 
 }
 void os__walk(string path, void (*fnc)( string path /*FFF*/ )) {
 
 if ( ! os__is_dir ( path ) ) {
 
 
 return ;
 
 }
 ;
 
Option_array_string tmp30 =  os__ls ( path ) ;
 if (!tmp30 .ok) {
 string err = tmp30 . error;
 int errcode = tmp30 . ecode;
 
 v_panic ( err ) ;
 
 }
 array_string files = *(array_string*) tmp30 . data;
 ;
 
 array_string tmp31 =  files;
 for (int tmp32 = 0; tmp32 < tmp31.len; tmp32++) {
 string file = ((string *) tmp31 . data)[tmp32];
 
 
string p=string_add(string_add( path , os__path_separator ) , file ) ;
 
 if ( os__is_dir ( p ) ) {
 
 os__walk ( p , fnc ) ;
 
 }
  else  if ( os__file_exists ( p ) ) {
 
 fnc ( p ) ;
 
 }
 ;
 
 }
 ;
 
 
 return ;
 
 }
 void os__signal(int signum, void* handler) {
 
 signal ( signum ,  handler ) ;
 
 }
 int os__fork() {
 
 #ifndef _WIN32
 
int pid= fork ( ) ;
 
return  pid ;
 
 #endif
 ;
 
 v_panic ( tos3("os.fork not supported in windows") ) ;
 
 }
 int os__wait() {
 
 #ifndef _WIN32
 
int pid= wait ( 0 ) ;
 
return  pid ;
 
 #endif
 ;
 
 v_panic ( tos3("os.wait not supported in windows") ) ;
 
 }
 int os__file_last_mod_unix(string path) {
  
 struct /*c struct init*/ 

stat attr ;
 
 stat ((char*) path .str ,  & attr ) ;
 
return  attr .st_mtime ;
 
 }
 void os__log(string s) {
 
 println (string_add( tos3("os.log: ") , s ) ) ;
 
 }
 void os__flush_stdout() {
 
 fflush ( stdout ) ;
 
 }
 void os__print_backtrace() {
 
 }
 void os__mkdir_all(string path) {
 
string p= ( string_starts_with ( path , os__path_separator ) ) ? ( os__path_separator ) : ( tos3("") ) ;
 
 array_string tmp33 =  string_split ( path , os__path_separator );
 for (int tmp34 = 0; tmp34 < tmp33.len; tmp34++) {
 string subdir = ((string *) tmp33 . data)[tmp34];
 
 
 p = string_add(p, string_add( subdir , os__path_separator ) ) ;
 
 if ( ! os__dir_exists ( p ) ) {
 
 os__mkdir ( p ) ;
 
 }
 ;
 
 }
 ;
 
 }
 string os__join(string base, _V_FnVargs_string *dirs) {
 
 println ( tos3("use filepath.join") ) ;
 
return  filepath__join ( base ,  dirs ) ;
 
 }
 array_string os__init_os_args(int argc, byteptr* argv) {
 
array_string args=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
 int tmp1 =  0;
 ;
for (int tmp2 = tmp1; tmp2 <  argc; tmp2++) {
 int i = tmp2;
 
 
_PUSH(& args , ( /*typ = array_string   tmp_typ=string*/ (tos2((byte *) argv [/*ptr*/ i ]/*rbyteptr 0*/ ) ) ), tmp3, string) ;
 
 }
 ;
 
return  args ;
 
 }
 string os__get_error_msg(int code) {
 
void* _ptr_text= strerror ( code ) ;
 
 if ( _ptr_text == 0 ) {
 
return  tos3("") ;
 
 }
 ;
 
return  tos ( _ptr_text , vstrlen ( _ptr_text ) ) ;
 
 }
 Option_array_string os__ls(string path) {
 
array_string res=new_array_from_c_array(0, 0, sizeof(string), EMPTY_ARRAY_OF_ELEMS( string, 0 ) {   TCCSKIP(0) }) ;
 
void* dir= opendir ((char*) path .str ) ;
 
 if ( isnil ( dir ) ) {
 
return  v_error ( _STR("ls() couldnt open dir \"%.*s\"", path.len, path.str) ) ;
 
 }
 ;
  
 struct /*c struct init*/ 

dirent* ent= 0 ;
 
 while (1) { 
 
 ent  =  readdir ( dir ) ;
 
 if ( isnil ( ent ) ) {
 
 break
 ;
 
 }
 ;
 
string name= tos_clone ( ((byteptr)( ent ->d_name ) ) ) ;
 
 if (string_ne( name , tos3(".") )  && string_ne( name , tos3("..") )  && string_ne( name , tos3("") ) ) {
 
_PUSH(& res , ( /*typ = array_string   tmp_typ=string*/ name ), tmp4, string) ;
 
 }
 ;
 
 }
 ;
 
 closedir ( dir ) ;
 
array_string tmp5 = OPTION_CAST(array_string)( res); 
 return opt_ok(&tmp5, sizeof(array_string)) ;
 
 }
 bool os__dir_exists(string path) {
 
void* dir= opendir ((char*) path .str ) ;
 
bool res= ! isnil ( dir ) ;
 
 if ( res ) {
 
 closedir ( dir ) ;
 
 }
 ;
 
return  res ;
 
 }
 void os__mkdir(string path) {
 
 mkdir ((char*) path .str ,  511 ) ;
 
 }
 string filepath__ext(string path) {
 
int pos= string_last_index_byte ( path , '.' ) ;
 
 if ( pos != - 1 ) {
 
return  string_substr2( path , pos , -1, true) ;
 
 }
 ;
 
return  tos3("") ;
 
 }
 bool filepath__is_abs(string path) {
 
 #ifdef _WIN32
 
return  string_at( path , 0) == '/'  ||  ( byte_is_letter ( string_at( path , 0) )  &&  string_at( path , 1) == ':' ) ;
 
 #endif
 ;
 
return  string_at( path , 0) == '/' ;
 
 }
 string filepath__join(string base, _V_FnVargs_string *dirs) {
 
strings__Builder path= strings__new_builder ( 50 ) ;
 
 strings__Builder_write (& /* ? */ path , string_trim_right ( base , tos3("\\/") ) ) ;
 
 for (int tmp12 = 0; tmp12 <  dirs->len; tmp12++) {
 string d = ((string *)  dirs->args)[tmp12];
 
 
 strings__Builder_write (& /* ? */ path , os__path_separator ) ;
 
 strings__Builder_write (& /* ? */ path , d ) ;
 
 }
 ;
 
return  strings__Builder_str ( path ) ;
 
 }
 int net__error_code() {
 
return  errno ;
 
 }
 Option_string net__hostname() {
 
byte name  [256 ] = { 0 } ;
 
void* res= gethostname ( & name ,  256 ) ;
 
 if ( res != 0 ) {
 
return  v_error ( tos3("net.hostname() cannot get the host name") ) ;
 
 }
 ;
 
string tmp1 = OPTION_CAST(string)( tos_clone ( name )); 
 return opt_ok(&tmp1, sizeof(string)) ;
 
 }
 Option_net__Socket net__socket(int family, int _type, int proto) {
 
void* sockfd= socket ( family ,  _type ,  proto ) ;
 
int one= 1 ;
 
 setsockopt ( sockfd ,  SOL_SOCKET ,  SO_REUSEADDR ,  & one ,  sizeof( int) ) ;
 
 if ( sockfd == 0 ) {
 
return  v_error ( tos3("socket: init failed") ) ;
 
 }
 ;
 
net__Socket s= (net__Socket) { .sockfd =  sockfd , .family =  family , ._type =  _type , .proto =  proto } ;
 
net__Socket tmp1 = OPTION_CAST(net__Socket)( s); 
 return opt_ok(&tmp1, sizeof(net__Socket)) ;
 
 }
 Option_net__Socket net__socket_udp() {
 
Option_net__Socket tmp2 = OPTION_CAST(Option_net__Socket)( net__socket ( AF_INET , SOCK_DGRAM , IPPROTO_UDP )); 
 return opt_ok(&tmp2, sizeof(net__Socket)) ;
 
 }
 Option_int net__Socket_setsockopt(net__Socket s, int level, int optname, int* optvalue) {
 
void* res= setsockopt ( s .sockfd ,  level ,  optname ,  optvalue ,   ( optvalue ) ) ;
 
 if ( res < 0 ) {
 
return  v_error ( tos3("socket: setsockopt failed") ) ;
 
 }
 ;
 
int tmp3 = OPTION_CAST(int)( ((int)( res ) )); 
 return opt_ok(&tmp3, sizeof(int)) ;
 
 }
 Option_int net__Socket_bind(net__Socket s, int port) {
  
 struct /*c struct init*/ 

sockaddr_in addr ;
 
 addr .sin_family  =  s .family ;
 
 addr .sin_port  =  htons ( port ) ;
 
 addr .sin_addr .s_addr  =  htonl ( INADDR_ANY ) ;
 
int size= 16 ;
 
int res= ((int)( bind ( s .sockfd ,  & addr ,  size ) ) ) ;
 
 if ( res < 0 ) {
 
return  v_error ( tos3("socket: bind failed") ) ;
 
 }
 ;
 
int tmp4 = OPTION_CAST(int)( res); 
 return opt_ok(&tmp4, sizeof(int)) ;
 
 }
 Option_int net__Socket_listen(net__Socket s) {
 
int backlog= 128 ;
 
int res= ((int)( listen ( s .sockfd ,  backlog ) ) ) ;
 
 if ( res < 0 ) {
 
return  v_error ( tos3("socket: listen failed") ) ;
 
 }
 ;
 
 #ifdef VDEBUG
 
printf( "listen res = %d\n", res ) ;
 
 #endif
 ;
 
int tmp5 = OPTION_CAST(int)( res); 
 return opt_ok(&tmp5, sizeof(int)) ;
 
 }
 Option_int net__Socket_listen_backlog(net__Socket s, int backlog) {
 
int n= 0 ;
 
 if ( backlog > 0 ) {
 
 n  =  backlog ;
 
 }
 ;
 
void* res= listen ( s .sockfd ,  n ) ;
 
 if ( res < 0 ) {
 
return  v_error ( tos3("socket: listen_backlog failed") ) ;
 
 }
 ;
 
int tmp6 = OPTION_CAST(int)( ((int)( res ) )); 
 return opt_ok(&tmp6, sizeof(int)) ;
 
 }
 Option_net__Socket net__listen(int port) {
 
 #ifdef VDEBUG
 
printf( "net.listen(%d)\n", port ) ;
 
 #endif
 ;
 
Option_net__Socket tmp7 =  net__socket ( AF_INET , SOCK_STREAM , 0 ) ;
 if (!tmp7 .ok) {
 string err = tmp7 . error;
 int errcode = tmp7 . ecode;
 
return  v_error ( err ) ;
 
 }
 net__Socket s = *(net__Socket*) tmp7 . data;
 ;
 
Option_int tmp8 =  net__Socket_bind ( s , port ) ;
 if (!tmp8 .ok) {
 string err = tmp8 . error;
 int errcode = tmp8 . ecode;
 
return  v_error ( err ) ;
 
 }
 int bind_res = *(int*) tmp8 . data;
 ;
 
Option_int tmp9 =  net__Socket_listen ( s ) ;
 if (!tmp9 .ok) {
 string err = tmp9 . error;
 int errcode = tmp9 . ecode;
 
return  v_error ( err ) ;
 
 }
 int listen_res = *(int*) tmp9 . data;
 ;
 
net__Socket tmp10 = OPTION_CAST(net__Socket)( s); 
 return opt_ok(&tmp10, sizeof(net__Socket)) ;
 
 }
 Option_net__Socket net__Socket_accept(net__Socket s) {
 
 #ifdef VDEBUG
 
 println ( tos3("accept()") ) ;
 
 #endif
 ;
  
 struct /*c struct init*/ 

sockaddr_storage addr ;
 
int size= 128 ;
 
void* sockfd= accept ( s .sockfd ,  & addr ,  & size ) ;
 
 if ( sockfd < 0 ) {
 
return  v_error ( tos3("socket: accept failed") ) ;
 
 }
 ;
 
net__Socket c= (net__Socket) { .sockfd =  sockfd , .family =  s .family , ._type =  s ._type , .proto =  s .proto } ;
 
net__Socket tmp11 = OPTION_CAST(net__Socket)( c); 
 return opt_ok(&tmp11, sizeof(net__Socket)) ;
 
 }
 Option_int net__Socket_connect(net__Socket s, string address, int port) {
  
 struct /*c struct init*/ 

addrinfo hints ;
 
 hints .ai_family  =  s .family ;
 
 hints .ai_socktype  =  s ._type ;
 
 hints .ai_flags  =  AI_PASSIVE ;
 
 hints .ai_protocol  =  s .proto ;
 
 hints .ai_addrlen  =  0 ;
 
 hints .ai_canonname  =  NULL ;
 
 hints .ai_addr  =  NULL ;
 
 hints .ai_next  =  NULL ;
  
 struct /*c struct init*/ 

addrinfo* info= 0 ;
 
string sport= _STR("%d", port) ;
 
void* info_res= getaddrinfo ((char*) address .str , (char*) sport .str ,  & hints ,  & info ) ;
 
 if ( info_res != 0 ) {
 
string error_message= os__get_error_msg ( net__error_code ( ) ) ;
 
return  v_error ( _STR("socket: getaddrinfo failed (%.*s)", error_message.len, error_message.str) ) ;
 
 }
 ;
 
int res= ((int)( connect ( s .sockfd ,  info ->ai_addr ,  info ->ai_addrlen ) ) ) ;
 
 if ( res < 0 ) {
 
string error_message= os__get_error_msg ( net__error_code ( ) ) ;
 
return  v_error ( _STR("socket: connect failed (%.*s)", error_message.len, error_message.str) ) ;
 
 }
 ;
 
int tmp12 = OPTION_CAST(int)( ((int)( res ) )); 
 return opt_ok(&tmp12, sizeof(int)) ;
 
 }
 Option_net__Socket net__dial(string address, int port) {
 
Option_net__Socket tmp13 =  net__socket ( AF_INET , SOCK_STREAM , 0 ) ;
 if (!tmp13 .ok) {
 string err = tmp13 . error;
 int errcode = tmp13 . ecode;
 
return  v_error ( err ) ;
 
 }
 net__Socket s = *(net__Socket*) tmp13 . data;
 ;
 
Option_int tmp14 =  net__Socket_connect ( s , address , port ) ;
 if (!tmp14 .ok) {
 string err = tmp14 . error;
 int errcode = tmp14 . ecode;
 
return  v_error ( err ) ;
 
 }
 int res = *(int*) tmp14 . data;
 ;
 
net__Socket tmp15 = OPTION_CAST(net__Socket)( s); 
 return opt_ok(&tmp15, sizeof(net__Socket)) ;
 
 }
 Option_int net__Socket_send(net__Socket s, byte* buf, int len) {
 
int res= ((int)( send ( s .sockfd , (char*) buf ,  len ,  0 ) ) ) ;
 
 if ( res < 0 ) {
 
return  v_error ( tos3("socket: send failed") ) ;
 
 }
 ;
 
int tmp16 = OPTION_CAST(int)( res); 
 return opt_ok(&tmp16, sizeof(int)) ;
 
 }
 _V_MulRet_byte_PTR__V_int net__Socket_recv(net__Socket s, int bufsize) {
 
byte* buf= v_malloc ( bufsize ) ;
 
int res= ((int)( recv ( s .sockfd , (char*) buf ,  bufsize ,  0 ) ) ) ;
 
return (_V_MulRet_byte_PTR__V_int){.var_0=buf,.var_1=res} ;
 
 }
 int net__Socket_cread(net__Socket s, byte* buffer, int buffersize) {
 
return  ((int)( read ( s .sockfd , (char*) buffer ,  buffersize ) ) ) ;
 
 }
 int net__Socket_crecv(net__Socket s, byte* buffer, int buffersize) {
 
return  ((int)( recv ( s .sockfd , (char*) buffer ,  buffersize ,  0 ) ) ) ;
 
 }
 Option_int net__Socket_close(net__Socket s) {
 
int shutdown_res= 0 ;
 
 #ifdef _WIN32
 
 shutdown_res  =  shutdown ( s .sockfd ,  SD_BOTH ) ;
 
 #else
 
 shutdown_res  =  shutdown ( s .sockfd ,  SHUT_RDWR ) ;
 
 #endif
 ;
 
int res= 0 ;
 
 #ifdef _WIN32
 
 res  =  closesocket ( s .sockfd ) ;
 
 #else
 
 res  =  close ( s .sockfd ) ;
 
 #endif
 ;
 
 if ( res < 0 ) {
 
return  v_error ( tos3("socket: close failed") ) ;
 
 }
 ;
 
int tmp17 = OPTION_CAST(int)( 0); 
 return opt_ok(&tmp17, sizeof(int)) ;
 
 }
 void net__Socket_write(net__Socket s, string str) {
 
string line= _STR("%.*s\r\n", str.len, str.str) ;
 
 send ( s .sockfd , (char*) line .str ,  line .len ,  0 ) ;
 
 }
 string net__Socket_read_line(net__Socket s) {
 
string res= tos3("") ;
 
 while (1) { 
 
 #ifdef VDEBUG
 
 println ( tos3(".") ) ;
 
 #endif
 ;
 
byte* buf= v_malloc ( net__MAX_READ ) ;
 
int n= ((int)( recv ( s .sockfd , (char*) buf ,  net__MAX_READ - 1 ,  0 ) ) ) ;
 
 #ifdef VDEBUG
 
printf( "numbytes=%d\n", n ) ;
 
 #endif
 ;
 
 if ( n == - 1 ) {
 
 #ifdef VDEBUG
 
 println ( tos3("recv failed") ) ;
 
 #endif
 ;
 
return  tos3("") ;
 
 }
 ;
 
 if ( n == 0 ) {
 
 break
 ;
 
 }
 ;
 
 buf [/*ptr*/ n ]/*rbyte 1*/  =  '\0' ;
 
string line= (tos2((byte *) buf ) ) ;
 
 res = string_add(res,  line ) ;
 
 if ( string_ends_with ( line , tos3("\n") )  ||  n < net__MAX_READ - 1 ) {
 
 break
 ;
 
 }
 ;
 
 if ( string_ends_with ( line , tos3("\r\n") ) ) {
 
 break
 ;
 
 }
 ;
 
 }
 ;
 
return  res ;
 
 }
 int net__Socket_get_port(net__Socket s) {
  
 struct /*c struct init*/ 

sockaddr_in addr ;
 
int size= 16 ;
 
void* sockname_res= getsockname ( s .sockfd ,  & addr ,  & size ) ;
 
return  ((int)( ntohs ( addr .sin_port ) ) ) ;
 
 }
 void Response_send_error(Response* res, int err_code) {
 
ResponseError v_error= (ResponseError) { .code =  err_code , .data =  tos3("") , .message =  tos3("") , } ;
  
 int tmp1 =  err_code; 

 v_error .message  =  ( ( tmp1 ==  main__JRPC_PARSE_ERROR ) ? ( tos3("Invalid JSON") ) : ( ( tmp1 ==  main__JRPC_INVALID_PARAMS ) ? ( tos3("Invalid params.") ) : ( ( tmp1 ==  main__JRPC_INVALID_REQUEST ) ? ( tos3("Invalid request.") ) : ( ( tmp1 ==  main__JRPC_METHOD_NOT_FOUND ) ? ( tos3("Method not found.") ) : ( ( tmp1 ==  main__JRPC_SERVER_ERROR_END ) ? ( tos3("Error while stopping the server.") ) : ( ( tmp1 ==  main__JRPC_SERVER_NOT_INITIALIZED ) ? ( tos3("Server not yet initialized.") ) : ( ( tmp1 ==  main__JRPC_SERVER_ERROR_START ) ? ( tos3("Error while starting the server.") ) :( tos3("Unknown error.") )))))))) ;
 
 res ->v_error  =  v_error ;
 
 }
 string Response_json(Response res) {
 
return  json__json_print(json__jsencode_Response( res)) ;
 
 }
 string ResponseError_str(ResponseError err) {
 
return  json__json_print(json__jsencode_ResponseError( err)) ;
 
 }
 void Response_send(Response* res, net__Socket conn) {
 
string res_json= Response_json (* res ) ;
 
 net__Socket_write ( conn , _STR("Content-Length: %d\r", res_json .len) ) ;
 
 net__Socket_write ( conn , tos3("") ) ;
 
 net__Socket_write ( conn , res_json ) ;
 
 }
 Request main__process_request(RawRequest raw_req) {
 
Request req= (Request) { main__JRPC_VERSION , raw_req .id , raw_req .method , new_map(1, sizeof(string)) } ;
 
array_string params_arr= string_split ( string_find_between ( raw_req .params , tos3("{") , tos3("}") ) , tos3(",") ) ;
 
 array_string tmp2 =  params_arr;
 for (int tmp3 = 0; tmp3 < tmp2.len; tmp3++) {
 string pkv = ((string *) tmp2 . data)[tmp3];
 
 
array_string p= string_split ( pkv , tos3(":") ) ;
 
string key= string_find_between ( ( *(string*) array_get(  p , 0) ) , tos3("\"") , tos3("\"") ) ;
 
string val= string_find_between ( ( *(string*) array_get(  p , 1) ) , tos3("\"") , tos3("\"") ) ;
 
map_set(& req .params , key , & (string []) {  val }) ;
 
 }
 ;
 
return  req ;
 
 }
 int Server_proc_index(Server server, string name) {
 
 array_Procedure tmp8 =  server .procs;
 for (int i = 0; i < tmp8.len; i++) {
 Procedure proc = ((Procedure *) tmp8 . data)[i];
 
 
 if (string_eq( proc .name , name ) ) {
 
return  i ;
 
 }
 ;
 
 }
 ;
 
return  - 1 ;
 
 }
 void Server_start(Server server) {
 
Option_net__Socket tmp9 =  net__listen ( server .port ) ;
 if (!tmp9 .ok) {
 string err = tmp9 . error;
 int errcode = tmp9 . ecode;
 
