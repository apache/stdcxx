//
// $Id$
//
// config.js - base classes for configurations support
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


//------------------------------------------------
//
// Common functions
//
//------------------------------------------------

// creates a copy of an object
function genericClone()
{
    return this.cloneTo(new Object(), true);
}

// copy all properties and optionally methods to a target object
function genericCloneTo(target, cloneMethods)
{
    //WScript.Echo(this);
    for (i in this)
    {
        var type = typeof(this[i]);
        //WScript.Echo(i + " " + type);
        switch (type)
        {
        case "object":
            target[i] = this[i].clone();
            break;
        case "number":
        case "boolean":
        case "undefined":
            target[i] = this[i];
            break;
        case "function":
            if (cloneMethods)
            {
                target[i] = this[i];
            }
            break;
        case "string":
            target[i] = String(this[i]);
            break;
        default:
            throw "Unknown property type.";
        }
    }
    return target;
}

//------------------------------------------------
// Associative Collection class
//------------------------------------------------

// Collection class .ctor
function Collection() {}

// Adds specified element to a collection of solutions
function addElement(name, element)
{
    this[name] = element;
}

// Removes specified element from a collection of solutions
function removeElement(name)
{
    delete this[name];
}

// Gets specified element from a collection of solutions
function getElement(name)
{
    return this[name];
}

// Collection class methods
Collection.prototype.cloneTo = genericCloneTo;
Collection.prototype.clone = genericClone;
Collection.prototype.add = addElement;
Collection.prototype.func_remove = removeElement;
Collection.prototype.get = getElement;

//------------------------------------------------
// ItemBuildInfo class
//------------------------------------------------

// ItemBuildInfo .ctor
function ItemBuildInfo(name)
{
    this.name = String(name);
    this.buildCmdLog = "";
    this.buildOutLog = "";
    this.errorsCnt = "0";
    this.warningsCnt = "0";
    this.linkerErrors = false;
    
    this.runOutput = "";
    this.runReqOutput = "";
    this.runDiff = "";
    this.exitCode = 0;
}

// ItemBuildInfo class methods
ItemBuildInfo.prototype.cloneTo = genericCloneTo;
ItemBuildInfo.prototype.clone = genericClone;
