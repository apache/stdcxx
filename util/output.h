/************************************************************************
 *
 * output.h - Interface declaration for the result parsing subsystem
 *
 * $Id$
 *
 ************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 * 
 **************************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

/**
   Umbrella (dispatch) function to analyse the (saved) output of target.

   This file opens the saved output for parsing.  If this fails, 
   the status structure is updated to reflect this.  Otherwise, check_test (),
   check_compat_test (), or check_example () is called to parse the output
   file.

   @param target the path to the executable that generated the output file 
   being parsed.
   @param status status object to record results in.
*/
void
parse_output (const struct target_opts* options, struct target_status* status);

#endif   /* OUTPUT_H */
