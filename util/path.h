#ifndef RWSTD_PATH_H_INCLUDED
#define RWSTD_PATH_H_INCLUDED

#include <string>


// creates a directory in the current working directory
// returns 0 on success, non-zero value on failure
int
makedir (const char*);


void
create_symlink (const std::string &dir, 
                const std::string &xname, 
                const std::string &sname);

// returns the pathname of a file `fname' which may be a file name
// or an absolute or relative pathname; relative pathnames are searched
// for in the following order
//   1.   if `other' is an absolute pathname, use dirname(other)
//   1.1. if `other' is a relative pathname, then use cwd() + dirname(other)
//        + fname
//   2.   relative to the ${RWSTD_SRC_ROOT} environment variable, if
//        it is set and non-empty
//   3.   relative to the current working directory
std::string
get_pathname (const std::string &fname,
              const std::string &other = std::string ());


#endif   // RWSTD_PATH_H_INCLUDED
