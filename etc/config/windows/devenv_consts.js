//
// $Id$
//
// devenv_consts.js - constants for VisualStudio Automation objects
//
//////////////////////////////////////////////////////////////////////
//
// Licensed to the Apache Software  Foundation (ASF) under one or more
// contributor  license agreements.  See  the NOTICE  file distributed
// with  this  work  for  additional information  regarding  copyright
// ownership.   The ASF  licenses this  file to  you under  the Apache
// License, Version  2.0 (the  "License"); you may  not use  this file
// except in  compliance with the License.   You may obtain  a copy of
// the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the  License is distributed on an  "AS IS" BASIS,
// WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
// implied.   See  the License  for  the  specific language  governing
// permissions and limitations under the License.
// 
//////////////////////////////////////////////////////////////////////

var typeUnknown = 0;
var typeApplication = 1;
var typeDynamicLibrary = 2;
var typeStaticLibrary = 4;
var typeGeneric = 10;
var typeLibrary = -1;

var charSetNotSet = 0; 
var charSetUnicode = 1; 
var charSetMBCS = 2;

var eFileTypeDefault = -1;
var eFileTypeCppCode = 0;
var eFileTypeCppClass = 1;
var eFileTypeCppHeader = 2;
var eFileTypeCppForm = 3;
var eFileTypeCppControl = 4;
var eFileTypeText = 5;
var eFileTypeDEF = 6;
var eFileTypeIDL = 7;
var eFileTypeMakefile = 8;
var eFileTypeRGS = 9;
var eFileTypeRC = 10;
var eFileTypeRES = 11;
var eFileTypeXSD = 12;
var eFileTypeXML = 13;
var eFileTypeHTML = 14;
var eFileTypeCSS = 15;
var eFileTypeBMP = 16;
var eFileTypeICO = 17;
var eFileTypeResx = 18;
var eFileTypeScript = 19;
var eFileTypeBSC = 20;
var eFileTypeXSX = 21;
var eFileTypeCppWebService = 22;
var eFileTypeAsax = 23;
var eFileTypeAspPage = 24;
var eFileTypeDocument = 25;
var eFileTypeDiscomap = 26;

var debugDisabled = 0;
var debugOldStyleInfo = 1;
var debugLineInfoOnly = 2;
var debugEnabled = 3;
var debugEditAndContinue = 4;

var warningLevel_0 = 0;
var warningLevel_1 = 1;
var warningLevel_2 = 2;
var warningLevel_3 = 3;
var warningLevel_4 = 4;

var optimizeDisabled = 0;
var optimizeMinSpace = 1;
var optimizeMaxSpeed = 2;
var optimizeFull = 3;
var optimizeCustom = 4;

var rtMultiThreaded = 0;
var rtMultiThreadedDebug = 1;
var rtMultiThreadedDLL = 2;
var rtMultiThreadedDebugDLL = 3;
var rtSingleThreaded = 4;
var rtSingleThreadedDebug = 5;

var pchNone = 0;
var pchCreateUsingSpecific = 1;
var pchGenerateAuto = 2;
var pchUseUsingSpecific = 3;

var linkIncrementalDefault = 0;
var linkIncrementalNo = 1;
var linkIncrementalYes = 2;

var optReferencesDefault = 0;
var optNoReferences = 1;
var optReferences = 2;

var optFoldingDefault = 0;
var optNoFolding = 1;
var optFolding = 2;

var machineNotSet = 0;
var machineX86 = 1;

var subSystemNotSet = 0;
var subSystemConsole = 1;
var subSystemWindows = 2;

var cppExceptionHandlingNo = 0;
var cppExceptionHandlingYes = 1;
var cppExceptionHandlingYesWithSEH = 2;

var preprocessNo = 0;
var preprocessYes = 1;
var preprocessNoLineNumbers = 2;

var vsWindowStateNormal = 0;
var vsWindowStateMinimize = 1;
var vsWindowStateMaximize = 2;

var vsSaveChangesYes = 1;
var vsSaveChangesNo = 2;
var vsSaveChangesPrompt = 3;

var runtimeBasicCheckNone = 0;
var runtimeCheckStackFrame = 1;
var runtimeCheckUninitVariables = 2;
var runtimeBasicCheckAll = 3;

var vsBuildStateNotStarted = 1;
var vsBuildStateInProgress = 2;
var vsBuildStateDone = 3;
