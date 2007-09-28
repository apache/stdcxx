//
// $Id$
//
// defines different utility functions
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

var VERSION = "";
var DEVENV = "";
var DEVENVFLAGS = "";
var CPPFLAGS = "";
var LDFLAGS = "";
var LIBS = "";
var CONVERT = false;
var CXX = "";
var LD = "";
var AR = "";
var AS = "";
var SLNVER="";
var SLNCOMMENT="";
var UNICODELOG = false;
var NOSTCRT = false;
var WINDIFF = "";
var ICCCONVERT = "";
var PLATFORM = "";
var CLVARSBAT = "";
var ICCVER="";

// timeout for exec utility in seconds
var EXEC_TIMEOUT = 300;

// expand system macros
function expandSysMacro(str)
{
    var env = null;
    var pos = 0;

    while (true)
    {
        pos = str.indexOf("%", pos);
        if (0 > pos)
            break;

        ++pos;
        var pos2 = str.indexOf("%", pos);
        if (0 > pos2)
            break;

        if (null == env)
            env = WshShell.Environment("PROCESS");

        var macro = str.substring(pos, pos2);
        pos = pos2 + 1;
        var value = env.Item(macro);
        if ("undefined" != typeof(value) && "" != value)
            str = str.replace("%" + macro + "%", value);
    }

    return str;
}

// read and parse compiler configuration file
// config - name of the compiler configuration
function parseConfig(config)
{
    var scriptDir = getParentFolder(WScript.ScriptFullName);
    var configFileName = scriptDir + "\\" + config + ".config";
    var ForReading = 1;
    var configFile = null;

    try
    {
        configFile = fso.OpenTextFile(configFileName, ForReading);
    }
    catch (e) {}

    if (null == configFile)
    {
        WScript.StdErr.WriteLine("Cannot open configuration file: " + configFileName);
        WScript.Quit(3);
    }

    var rx = /^\s*([A-Z]+)\s*=\s*(\S.*)$/;

    while (!configFile.AtEndOfStream)
    {
        var line = configFile.ReadLine();

        if (0 == line.length || 0 == line.indexOf("//"))
            continue;

        var inc = "#include ";
        var pos = line.indexOf(inc);
        if (0 == pos)
            parseConfig(line.substr(pos + inc.length));

        var arr = rx.exec(line);
        if (null == arr || 0 == arr[2].length)
            continue;

        arr[2] = expandSysMacro(arr[2]);

        switch (arr[1])
        {
        case "VERSION":
            VERSION = arr[2];
            break;
        case "DEVENV":
            DEVENV = arr[2];
            break;
        case "DEVENVFLAGS":
            DEVENVFLAGS = arr[2];
            break;
        case "CPPFLAGS":
            CPPFLAGS = arr[2];
            break;
        case "LDFLAGS":
            LDFLAGS = arr[2];
            break;
        case "LIBS":
            LIBS = arr[2];
            break;
        case "CONVERT":
            CONVERT = parseInt(arr[2]) != 0;
            break;
        case "CXX":
            CXX = arr[2];
            break;
        case "LD":
            LD = arr[2];
            break;
        case "AR":
            AR = arr[2];
            break;
        case "AS":
            AS = arr[2];
            break;
        case "SLNVER":
            SLNVER = arr[2];
            break;
        case "SLNCOMMENT":
            SLNCOMMENT = arr[2];
            break;
        case "UNICODELOG":
            UNICODELOG = parseInt(arr[2]) != 0;
            break;
        case "NOSTCRT":
            NOSTCRT = parseInt(arr[2]) != 0;
            break;
        case "WINDIFF":
            WINDIFF = arr[2];
            break;
        case "ICCCONVERT":
            ICCCONVERT = arr[2];
            break;
        case "PLATFORM":
            PLATFORM = arr[2];
            break;
        case "CLVARSBAT":
            CLVARSBAT = arr[2];
            break;
        case "ICCVER":
            ICCVER = arr[2];
            break;
        }
    }
}

// init script variables for specified compiler configuration
function getCompilerOpts(config)
{
    // set vars to initial state
    VERSION = "";
    DEVENV = "";
    DEVENVFLAGS = "";
    CPPFLAGS = "";
    LDFLAGS = "";
    LIBS = "";
    CONVERT = false;
    CXX = "";
    LD = "";
    AR = "";
    AS = "";
    SLNVER="";
    SLNCOMMENT="";
    UNICODELOG = false;
    NOSTCRT = false;
    WINDIFF = "";
    ICCCONVERT = "";
    PLATFORM = "";
    CLVARSBAT = "";
    ICCVER = "";

    parseConfig(config);

    if (0 == WINDIFF.length)
    {
        if (VERSION.length)
        {
            var ver = "";
            if (0 <= VERSION.indexOf("."))
                ver = VERSION.replace(".", "");
            var path = WshShell.Environment.Item("VS" + ver + "COMNTOOLS");
            if (path.length)
            {
                WINDIFF = "\"" +
                    fso.BuildPath(path.replace(/\"/g, ""), "bin\\windiff") +
                    "\"";
            }
        }

        if (0 == WINDIFF.length)
            WINDIFF = "windiff";
    }
}

// out variables and their values to the stream
function PrintVars(stream)
{
    stream.WriteLine("Variables:");
    stream.WriteLine("  VERSION=" + VERSION);
    stream.WriteLine("  DEVENV=" + DEVENV);
    stream.WriteLine("  DEVENVFLAGS=" + DEVENVFLAGS);
    stream.WriteLine("  CPPFLAGS=" + CPPFLAGS);
    stream.WriteLine("  LDFLAGS=" + LDFLAGS);
    stream.WriteLine("  LIBS=" + LIBS);
    stream.WriteLine("  CONVERT=" + CONVERT);
    stream.WriteLine("  CXX=" + CXX);
    stream.WriteLine("  LD=" + LD);
    stream.WriteLine("  AR=" + AR);
    stream.WriteLine("  AS=" + AS);
    stream.WriteLine("  SLNVER=" + SLNVER);
    stream.WriteLine("  SLNCOMMENT=" + SLNCOMMENT);
    stream.WriteLine("  UNICODELOG=" + UNICODELOG);
    stream.WriteLine("  NOSTCRT=" + NOSTCRT);
    stream.WriteLine("  WINDIFF=" + WINDIFF);
    stream.WriteLine("  ICCCONVERT=" + ICCCONVERT);
    stream.WriteLine("  PLATFORM=" + PLATFORM);
    stream.WriteLine("  CLVARSBAT=" + CLVARSBAT);
    stream.WriteLine("  ICCVER=" + ICCVER);
    stream.WriteLine("");
}

// returns parent folder for a path
function getParentFolder(path)
{
    var idx = path.lastIndexOf('\\');
    return path.substr(0, idx);
}

// returns filename from a path
function getFileName(path)
{
    var idx = path.lastIndexOf('\\');
    return path.substr(idx + 1);
}

// returns extension of the file name (with dot at first character)
function getExtension(filename)
{
    var idx = filename.lastIndexOf('.');
    return 0 <= idx ? filename.substr(idx) : ".";
}

/////////////////////////////////////////////////////////////////////////
// get differences using WinDiff utility

function getWinDiffDifferences(src1, src2, flags)
{   
    try
    {
        // first create two temporary files 
        var tfolder, TemporaryFolder = 2;
        tfolder = fso.GetSpecialFolder(TemporaryFolder);
        if (! tfolder)
            return "unknown";
    
        var tname1 = fso.GetTempName();
        var tfile1 = tfolder.CreateTextFile(tname1);
        var tpath1 = tfolder.Path + "\\" + tname1;
        tfile1.Write(src1);
        tfile1.Close();
  
        var tname2 = fso.GetTempName();
        var tfile2 = tfolder.CreateTextFile(tname2);
        var tpath2 = tfolder.Path + "\\" + tname2;
        tfile2.Write(src2);
        tfile2.Close();
  
        var tResName = fso.GetTempName();
        var tResPath = tfolder.Path + "\\" + tResName;
  
        // second run windiff
        var cmd = WINDIFF + " -F" + flags + " " + tResPath;
        cmd += " " + tpath1;
        cmd += " " + tpath2;

        var result = WshShell.Run(cmd, 7, true);
        if (result != 0)
        {
            WScript.StdErr.WriteLine(
                "getWinDiffDifferences: Fatal error: windiff"
                    + " failed");                
            return "unknown";
        }
    
        // third read the results
        var tResFile = fso.OpenTextFile(tfolder.Path + "\\" + tResName);
        var res = tResFile.ReadAll();
        tResFile.Close();
    
        fso.DeleteFile(tpath1);
        fso.DeleteFile(tpath2);
        fso.DeleteFile(tResPath);
    
        return res;
    }
    catch(e)
    {
        return "Information about differences is not available";
    }
}

// create temporary file and return path to this file
function makeTempFile()
{
    var tfolder, TemporaryFolder = 2;
    tfolder = fso.GetSpecialFolder(TemporaryFolder);
    
    var tname = fso.GetTempName();
    var tfile = tfolder.CreateTextFile(tname);
    tfile.Close();
    
    return tfolder.Path + "\\" + tname;
}

// encode symbols within string with escaped analog
// srcString - source string
function encodeHTML(srcString)
{
    var res = srcString;
    var srcS = new Array ( "&", "<", ">", "\"" );
    var destS = new Array ( "&amp;", "&lt;", "&gt;", "&quot;" );
    for (var t = 0; t < srcS.length; t++)
    {
        var reRep = new RegExp(srcS[t], "g");
        res = res.replace(reRep, destS[t]);
    }
    
    return res;
}

// decode escaped symbols within string with analog
// srcString - source string
function decodeHTML(srcString)
{
    var res = srcString;
    var srcS = new Array ( "&amp;", "&lt;", "&gt;", "&quot;", "&nbsp;" );
    var destS = new Array ( "&", "<", ">", "\"", " " );
    for (var t = 0; t < srcS.length; t++)
    {
        var reRep = new RegExp(srcS[t], "g");
        res = res.replace(reRep, destS[t]);
    }
    
    return res;
}

// remove all tags within string
// srcString - source string
function stripTags(srcString)
{
    return decodeHTML(srcString).replace(new RegExp("> <", "g"), "><")
           .replace(new RegExp("><", "g"), ">\r\n<")
           .replace(new RegExp("(^<[\\s\\S]+?>)", "gm"), "")
           .replace(new RegExp("\r", "g"), "")
           .replace(new RegExp("\n{2,}", "g"), "\n\n")
           .replace(new RegExp("</pre>", "g"), "")
           .replace(new RegExp("</h3>", "g"), "")
           .replace(new RegExp("</font>", "g"), "");
}

// returns source string without first character if it equal to symbol
// dotName - source string
// symbol - symbol to remove
function removeLeadingSymbol(dotName, symbol)
{
    var index = dotName.indexOf(symbol);
    if (0 != index)
        return dotName;
        
    var resName = dotName.substr(symbol.length);
    return resName;
}

// returns source string without first character is is a dot
function removeLeadingDot(dotName)
{
    var index = dotName.indexOf(".");
    if (0 != index)
        return dotName;
        
    var resName = dotName.substr(1);
    return resName;
}

// returns the source filename with replaced extension to new one
// filename - source filename
// ext - new extension
function changeFileExt(filename, ext)
{
    var arr = filename.split(".");
    arr[arr.length - 1] = ext;
    return arr.join(".");
}

// returns new UUID
function createUUID()
{
    return WScript.CreateObject("scriptlet.typelib").guid;
}

// returns index of value in array or -1
function arrayIndexOf(array, value)
{
    for (var i = 0; i < array.length; ++i)
        if (array[i] == value)
            return i;

    return -1;
}

// create MSVS solution file
// projects - array of the projects
// path - output folder
// name - name of the solutuion file
function generateSolution(projects, path, name)
{
    path += "\\";
    var sln = fso.CreateTextFile(path + name, true, false);
    // header
    sln.WriteLine("Microsoft Visual Studio Solution File, Format Version " + SLNVER);
    if (0 < SLNCOMMENT.length)
        sln.WriteLine("# " + SLNCOMMENT);
    for (var i = 0; i < projects.length; ++i)
    {
        // project section header
        var Project = projects[i];
        var VCProject = Project.VSProject;
        sln.Write("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = ");
        sln.Write("\"" + VCProject.Name + "\"");
        var projectFile = VCProject.ProjectFile;
        var pos = projectFile.indexOf(path);
        if (0 == pos)
            projectFile = projectFile.substr(path.length);
        sln.Write(", \"" + projectFile + "\"");
        sln.WriteLine(", \"" + VCProject.ProjectGUID + "\"");

        if (SLNVER != "7.00")
        {
            // project dependencies
            sln.WriteLine(
                "\tProjectSection(ProjectDependencies) = postProject");
            var deps = Project.PrjRefs.concat(Project.PrjDeps);
            for (var j = 0; j < deps.length; ++j)
            {
                var depGUID = deps[j].VSProject.ProjectGUID;
                sln.WriteLine("\t\t" + depGUID + " = " + depGUID);
            }
            sln.WriteLine("\tEndProjectSection");
        }
        // project section end
        sln.WriteLine("EndProject");
    }

    // Global section
    sln.WriteLine("Global");
    // solution configurations
    sln.WriteLine("\tGlobalSection(SolutionConfiguration) = preSolution");
    for (var i = 0; i < confNames.length; ++i)
    {
        var confName = confNames[i];
        var confKey = (SLNVER == "7.00" ? "ConfigName." + i : confName);
        sln.WriteLine("\t\t" + confKey + " = " + confName);
    }
    sln.WriteLine("\tEndGlobalSection");

    // project dependencies for MSVC 7.0
    if (SLNVER == "7.00")
    {
        sln.WriteLine("\tGlobalSection(ProjectDependencies) = postSolution");
        for (var i = 0; i < projects.length; ++i)
        {
            var Project = projects[i];
            var VCProject = Project.VSProject;
            var prjGUID = VCProject.ProjectGUID;
            var deps = Project.PrjRefs.concat(Project.PrjDeps);
            for (var j = 0; j < deps.length; ++j)
            {
                var depGUID = deps[j].VSProject.ProjectGUID;
                sln.WriteLine("\t\t" + prjGUID + "." + j + " = " + depGUID);
            }
        }
        sln.WriteLine("\tEndGlobalSection");
    }

    // project configurations
    sln.WriteLine("\tGlobalSection(ProjectConfiguration) = postSolution");
    for (var i = 0; i < projects.length; ++i)
    {
        var Project = projects[i];
        var VCProject = Project.VSProject;
        var prjGUID = VCProject.ProjectGUID;
        var cfgs = VCProject.Configurations;
        for (var j = 1; j <= cfgs.Count; ++j)
        {
            var cfg = cfgs.Item(j);
            sln.WriteLine("\t\t" + prjGUID + "." + cfg.ConfigurationName +
                          ".ActiveCfg = " + cfg.Name);
            sln.WriteLine("\t\t" + prjGUID + "." + cfg.ConfigurationName +
                          ".Build.0 = " + cfg.Name);
        }
    }
    sln.WriteLine("\tEndGlobalSection");

    // some unknown stuff
    sln.WriteLine("\tGlobalSection(ExtensibilityGlobals) = postSolution");
    sln.WriteLine("\tEndGlobalSection");
    sln.WriteLine("\tGlobalSection(ExtensibilityAddIns) = postSolution");
    sln.WriteLine("\tEndGlobalSection");

    sln.WriteLine("EndGlobal");
    sln.Close();
}
