//
// $Id$
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

// base configuration settings

//------------------------------------------------
// object storing info about configuration
//------------------------------------------------

function ConfigInfo(debug, mt, dll, out)
{
    this.debug = debug;
    this.mt = mt;
    this.dll = dll;
    this.out = out;
}

// configuration names
var confDebugStaticName = "11s Debug Static";
var confReleaseStaticName = "8s Optimized Static";
var confMTDebugStaticName = "15s Debug Thread-safe Static";
var confMTReleaseStaticName = "12s Optimized Thread-safe Static";
var confDebugDllName = "11d Debug Dll";
var confReleaseDllName = "8d Optimized Dll";
var confMTDebugDllName = "15d Debug Thread-safe Dll";
var confMTReleaseDllName = "12d Optimized Thread-safe Dll";

// configuration output directories
var confDebugStaticOut = "11s";
var confReleaseStaticOut = "8s";
var confMTDebugStaticOut = "15s";
var confMTReleaseStaticOut = "12s";
var confDebugDllOut = "11d";
var confReleaseDllOut = "8d";
var confMTDebugDllOut = "15d";
var confMTReleaseDllOut = "12d";

var configs = new Collection();
configs.add(confDebugStaticName, new ConfigInfo(true, false, false, confDebugStaticOut));
configs.add(confReleaseStaticName, new ConfigInfo(false, false, false, confReleaseStaticOut));
configs.add(confMTDebugStaticName, new ConfigInfo(true, true, false, confMTDebugStaticOut));
configs.add(confMTReleaseStaticName, new ConfigInfo(false, true, false, confMTReleaseStaticOut));
configs.add(confDebugDllName, new ConfigInfo(true, false, true, confDebugDllOut));
configs.add(confReleaseDllName, new ConfigInfo(false, false, true, confReleaseDllOut));
configs.add(confMTDebugDllName, new ConfigInfo(true, true, true, confMTDebugDllOut));
configs.add(confMTReleaseDllName, new ConfigInfo(false, true, true, confMTReleaseDllOut));

var confStaticNames = new Array(
    confDebugStaticName, confReleaseStaticName,
    confMTDebugStaticName, confMTReleaseStaticName);

var confDllNames = new Array(
    confDebugDllName, confReleaseDllName,
    confMTDebugDllName, confMTReleaseDllName);

var confDebugNames = new Array(
    confDebugStaticName, confMTDebugStaticName,
    confDebugDllName, confMTDebugDllName);

var confReleaseNames = new Array(
    confReleaseStaticName, confMTReleaseStaticName,
    confReleaseDllName, confMTReleaseDllName);

var confNames = confStaticNames.concat(confDllNames);
