# executables_finder

a neat little tool that flat copies (no subdirectories, names containing subdirectory paths) all executables (.dll & .exe files) from one directory into another through deep searching (including all executables from the directory's tree). if you need all files from some shady software analysed by an antivirus like virustotal this is the tool you're looking for.

### usage
`executables_finder.exe INPUT_DIRECTORY OUTPUT_DIRECTORY` \
(both relative and full paths are supported)

### example output
```
parsing...
found 2293 files
copying...
2293/2293 files copied - 0 failed
copied 3332.68MB
Press any key to continue . . .
```

### compiling
#### tested specs :
- windows 10
- mingw-w64 v11.0 (glibc)
- CMake v3.27.8
#### compatibility
this should compile under mingw-w64, cygwin, linux, and any other both POSIX and C11 compliant system, although it has been tested so have fun at your own risk. 