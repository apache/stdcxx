//
// $Id$
//
// projects.js - Definitions of the solution projects
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

var commonDefines = "debug?_RWSTDDEBUG;dll?_RWSHARED";
var commonIncludes = "$(SolutionDir)%CONFIG%\\include";
var stdcxxIncludes = "%SRCDIR%\\include;%SRCDIR%\\include\\ansi;" + commonIncludes;
var rwtestIncludes = "%SRCDIR%\\tests\\include;" + stdcxxIncludes;

var binPath = "$(SolutionDir)%CONFIG%\\bin";
var libPath = "$(SolutionDir)%CONFIG%\\lib";

var ProjectsDir = "%BUILDDIR%\\Projects";

// projects which requires RTTI support
var NonRTTIProjects = new Array();

var rxExcludedFolders = 
    new RegExp("^(?:\\.svn|Release.*|Debug.*|in|out|CVS)$","i");

// fill and return array of ProjectDef objects
// with definitions of the solution projects
// copyDll - if true then libstd and rwtest dlls will be copied
//           to the target folder after build
// buildLocales - if true then generate projects for build locales
// testLocales - if true then generate projects for test locales
function CreateProjectsDefs(copyDll, buildLocales, testLocales)
{
    var projectDefs = new Array();
 
///////////////////////////////////////////////////////////////////////////////
    var configureDef = new ProjectDef(".configure", typeGeneric);
    configureDef.VCProjDir = ProjectsDir;
    configureDef.FilterDefs.push(
        new FilterDef(sourceFilterName, sourceFilterUuid, ".cpp;.c;.cxx;.s;.cc", eFileTypeCppCode, false).
            addFilesByMask("%SRCDIR%\\etc\\config\\src", rxExcludedFolders, null));
    configureDef.FilterDefs.push(
        new FilterDef(headerFilterName, headerFilterUuid, ".h;.hpp;.hxx", eFileTypeCppHeader, false).
            addFilesByMask("%SRCDIR%\\etc\\config\\src", rxExcludedFolders, null));
    configureDef.FilterDefs.push(
        new FilterDef("Script Files", null, ".js;.wsf", eFileTypeScript, false).
            addFiles("%SRCDIR%\\etc\\config\\windows",
                new Array("configure.wsf", "config.js", "data.js", "utilities.js")));
    configureDef.OutDir = commonIncludes;
    configureDef.IntDir = configureDef.OutDir;
    configureDef.CustomBuildFile = "configure.wsf";
    if (0 < CLVARSBAT.length)
        configureDef.CustomBuildCmd = "echo Calling \"" + CLVARSBAT + "\"\r\n" +
                                      "call \"" + CLVARSBAT + "\"\r\n";
    else
        configureDef.CustomBuildCmd = "";
    configureDef.CustomBuildCmd += "cscript /nologo \"%CUSTOMFILE%\"" +
        " /SolutionName:\"%SOLUTION%\"" +
        " /ConfigurationName:\"%CFGNAME%\"" +
        " /SrcDir:\"%SRCDIR%\\etc\\config\\src\"" +
        " /OutDir:\"$(OutDir)\"" +
        " /OutFile:\"$(OutDir)\\config.h\"" +
        " /LogFile:\"$(OutDir)\\config.log\"";
    configureDef.CustomBuildOut = "$(OutDir)\\config.h";
    configureDef.CustomBuildDeps = "%FILES%";

    projectDefs.push(new Array(configureDef));

///////////////////////////////////////////////////////////////////////////////
    var stdcxxDef = new ProjectDef(".stdcxx", typeLibrary);
    stdcxxDef.VCProjDir = ProjectsDir;
    stdcxxDef.FilterDefs.push(
        new FilterDef(sourceFilterName, sourceFilterUuid, sourceFilterExts + ";.inc", eFileTypeCppCode, false).
            addFilesByMask("%SRCDIR%\\src", rxExcludedFolders, null));
    stdcxxDef.FilterDefs.push(
        new FilterDef(headerFilterName, headerFilterUuid, headerFilterExts, eFileTypeCppHeader, true).
            addFilesByMask("%SRCDIR%\\src", rxExcludedFolders, null).
            addFilter(new FilterDef("Include", headerFilterUuid, headerFilterExts + ";.", eFileTypeCppHeader, true).
                addFilesByMask("%SRCDIR%\\include", rxExcludedFolders, null)));
    stdcxxDef.Defines = commonDefines;
    stdcxxDef.Includes = stdcxxIncludes;
    stdcxxDef.OutDir = libPath;
    stdcxxDef.IntDir = "$(SolutionDir)%CONFIG%\\src";
    stdcxxDef.Libs = LIBS;
    stdcxxDef.OutFile = "$(OutDir)\\libstd%CONFIG%%EXT%";
    stdcxxDef.PrjDeps.push(configureDef);

    projectDefs.push(new Array(stdcxxDef));

///////////////////////////////////////////////////////////////////////////////
    var rwtestDef = new ProjectDef(".rwtest", typeLibrary);
    rwtestDef.VCProjDir = ProjectsDir;
    rwtestDef.FilterDefs.push(
        new FilterDef(sourceFilterName, sourceFilterUuid, sourceFilterExts, eFileTypeCppCode, false).
            addFilesByMask("%SRCDIR%\\tests\\src", rxExcludedFolders, null));
    rwtestDef.FilterDefs.push(
        new FilterDef(headerFilterName, headerFilterUuid, headerFilterExts, eFileTypeCppHeader, true).
            addFilesByMask("%SRCDIR%\\tests\\src", rxExcludedFolders, null).
            addFilter(new FilterDef("Include", headerFilterUuid, headerFilterExts, eFileTypeCppHeader, true).
                addFilesByMask("%SRCDIR%\\tests\\include", rxExcludedFolders, null)));
    rwtestDef.Defines = commonDefines;
    rwtestDef.Includes = rwtestIncludes;
    rwtestDef.OutDir = "$(SolutionDir)%CONFIG%\\tests";
    rwtestDef.IntDir = rwtestDef.OutDir + "\\src";
    rwtestDef.Libs = LIBS;
    rwtestDef.PrjRefs.push(stdcxxDef);

    projectDefs.push(new Array(rwtestDef));

///////////////////////////////////////////////////////////////////////////////
    var utilsArray = new Array();

    var execDef = new ProjectDef("util_exec", typeApplication);
    execDef.VCProjDir = ProjectsDir + "\\util";
    execDef.FilterDefs.push(
        new FilterDef(sourceFilterName, sourceFilterUuid, sourceFilterExts, eFileTypeCppCode, false).
            addFiles("%SRCDIR%\\util",
                new Array("cmdopt.cpp", "display.cpp", "exec.cpp", "output.cpp", "runall.cpp", "util.cpp")));
    execDef.FilterDefs.push(
        new FilterDef(headerFilterName, headerFilterUuid, headerFilterExts, eFileTypeCppHeader, true).
            addFiles("%SRCDIR%\\util",
                new Array("cmdopt.h", "display.h", "exec.h", "output.h", "target.h", "util.h")));
    execDef.Defines = "";
    execDef.Includes = commonIncludes;
    execDef.OutDir = binPath;
    execDef.Libs = LIBS;
    execDef.OutFile = "$(OutDir)\\exec.exe";
    execDef.PrjDeps.push(configureDef);

    utilsArray.push(execDef);

///////////////////////////////////////////////////////////////////////////////
    var localedefDef = new ProjectDef("util_localedef", typeApplication);
    localedefDef.VCProjDir = ProjectsDir + "\\util";
    localedefDef.FilterDefs.push(
        new FilterDef(sourceFilterName, sourceFilterUuid, sourceFilterExts, eFileTypeCppCode, false).
            addFiles("%SRCDIR%\\util",
                new Array("aliases.cpp", "charmap.cpp", "codecvt.cpp",
                          "collate.cpp", "ctype.cpp", "def.cpp",
                          "diagnostic.cpp", "locale.cpp", "localedef.cpp",
                          "memchk.cpp", "messages.cpp", "monetary.cpp",
                          "numeric.cpp", "path.cpp", "scanner.cpp",
                          "time.cpp")));
    localedefDef.FilterDefs.push(
        new FilterDef(headerFilterName, headerFilterUuid, headerFilterExts, eFileTypeCppHeader, true).
            addFiles("%SRCDIR%\\util",
                new Array("aliases.h", "charmap.h", "def.h", "diagnostic.h",
                          "loc_exception.h", "localedef.h", "memchk.h",
                          "path.h", "scanner.h")));
    localedefDef.Defines = commonDefines;
    localedefDef.Includes = stdcxxIncludes;
    localedefDef.OutDir = binPath;
    localedefDef.Libs = LIBS;
    localedefDef.OutFile = "$(OutDir)\\localedef.exe";
    localedefDef.PrjRefs.push(stdcxxDef);

    utilsArray.push(localedefDef);

///////////////////////////////////////////////////////////////////////////////
    var localeDef = new ProjectDef("util_locale", typeApplication);
    localeDef.VCProjDir = ProjectsDir + "\\util";
    localeDef.FilterDefs.push(
        new FilterDef(sourceFilterName, sourceFilterUuid, sourceFilterExts, eFileTypeCppCode, false).
            addFiles("%SRCDIR%\\util",
                new Array("locale_stub.cpp")));
    localeDef.Defines = commonDefines;
    localeDef.Includes = stdcxxIncludes;
    localeDef.OutDir = binPath;
    localeDef.Libs = LIBS;
    localeDef.OutFile = "$(OutDir)\\locale.exe";
    localeDef.PrjDeps.push(configureDef);

    utilsArray.push(localeDef);

///////////////////////////////////////////////////////////////////////////////
    var gencatDef = new ProjectDef("util_gencat", typeApplication);
    gencatDef.VCProjDir = ProjectsDir + "\\util";
    gencatDef.FilterDefs.push(
        new FilterDef(sourceFilterName, sourceFilterUuid, sourceFilterExts, eFileTypeCppCode, false).
            addFiles("%SRCDIR%\\util",
                new Array("gencat.cpp")));
    gencatDef.Defines = commonDefines;
    gencatDef.Includes = stdcxxIncludes;
    gencatDef.OutDir = binPath;
    gencatDef.Libs = LIBS;
    gencatDef.OutFile = "$(OutDir)\\gencat.exe";
    gencatDef.PrjRefs.push(stdcxxDef);

    utilsArray.push(gencatDef);

///////////////////////////////////////////////////////////////////////////////
    var utilsDef = new ProjectDef(".stdcxx_utils", typeGeneric);
    utilsDef.VCProjDir = ProjectsDir + "\\util";
    utilsDef.OutDir = binPath;
    utilsDef.IntDir = utilsDef.OutDir;
    utilsDef.PrjDeps.push(execDef);
    utilsDef.PrjDeps.push(localedefDef);
    utilsDef.PrjDeps.push(localeDef);
    utilsDef.PrjDeps.push(gencatDef);

    utilsArray.push(utilsDef);

    projectDefs.push(utilsArray);

///////////////////////////////////////////////////////////////////////////////
    var exampleArray = new Array();

    var exampleTplDef = new ProjectDef(null, typeApplication);
    exampleTplDef.VCProjDir = ProjectsDir + "\\examples";
    exampleTplDef.Defines = commonDefines;
    exampleTplDef.Includes = "%SRCDIR%\\examples\\include;" + stdcxxIncludes;
    exampleTplDef.OutDir = "$(SolutionDir)%CONFIG%\\examples";
    exampleTplDef.Libs = LIBS;
    exampleTplDef.PrjRefs.push(stdcxxDef);
    
    var exampleDefs = exampleTplDef.createProjectDefsFromFolder(
        "%SRCDIR%\\examples",
        new RegExp("^.+\\.(?:cpp)$", "i"),
        new RegExp("^(?:\\.svn|Release.*|Debug.*|in|out|CVS)$", "i"),
        new RegExp("^(?:rwstdmessages.cpp)$", "i"), false);

    exampleArray = exampleArray.concat(exampleDefs);
    
///////////////////////////////////////////////////////////////////////////////
    var allexamplesDef = new ProjectDef(".stdcxx_examples", typeGeneric);
    allexamplesDef.VCProjDir = ProjectsDir + "\\examples";
    allexamplesDef.OutDir = "$(SolutionDir)%CONFIG%\\examples";
    allexamplesDef.IntDir = allexamplesDef.OutDir;
    allexamplesDef.PrjDeps = exampleDefs;

    exampleArray.push(allexamplesDef);

    projectDefs.push(exampleArray);

///////////////////////////////////////////////////////////////////////////////
    var runexamplesDef = new ProjectDef(".stdcxx_runexamples", typeGeneric);
    runexamplesDef.VCProjDir = ProjectsDir + "\\examples";
    runexamplesDef.FilterDefs.push(
        new FilterDef("Script Files", null, ".js;.wsf", eFileTypeScript, false).
            addFiles("%SRCDIR%\\etc\\config\\windows",
                new Array("runall.wsf", "config.js", "utilities.js",
                          "summary.js")));
    runexamplesDef.OutDir = "$(SolutionDir)%CONFIG%\\examples";
    runexamplesDef.IntDir = runexamplesDef.OutDir;
    runexamplesDef.PreBuildCmd =
        "if exist \"$(OutDir)\\summary.htm del\" \"$(OutDir)\\summary.htm\"\r\n" +
        "if exist \"$(OutDir)\\runexamples.log\" del \"$(OutDir)\\runexamples.log\"";
    runexamplesDef.CustomBuildFile = "runall.wsf";
    runexamplesDef.CustomBuildCmd =
        "set PATH=$(SolutionDir)%CONFIG%\\bin;$(SolutionDir)%CONFIG%\\lib;" +
        "%SRCDIR%\\examples\\manual;%PATH%\r\n" +
        "set TZ=MST+7\r\n" +
        "cscript /nologo \"%CUSTOMFILE%\"" +
        " /INOUTDIR:\"%SRCDIR%\\examples\"" +
        " /EXEDIR:\"$(OutDir)\"" +
        " /PRJDIR:\"" + runexamplesDef.VCProjDir + "\"" +
        " /CONFIG:\"%SOLUTION%\"" +
        " /LOGFILE:\"runexamples.log\"" +
        " /RUNFLAGS:\"-t " + EXEC_TIMEOUT + "\"";
    runexamplesDef.CustomBuildOut = "$(OutDir)\\runexamples.log";
    runexamplesDef.CustomBuildDeps = "%FILES%";
    //runexamplesDef.PrjDeps.push(allexamplesDef);
    runexamplesDef.PrjDeps.push(execDef);

    projectDefs.push(new Array(runexamplesDef));

///////////////////////////////////////////////////////////////////////////////
    var testArray = new Array();

    var testTplDef = new ProjectDef(null, typeApplication);
    testTplDef.VCProjDir = ProjectsDir + "\\tests";
    testTplDef.Defines = commonDefines;
    testTplDef.Includes = rwtestIncludes;
    testTplDef.OutDir = "$(SolutionDir)%CONFIG%\\tests";
    testTplDef.Libs = LIBS;
    testTplDef.PrjRefs.push(stdcxxDef);
    testTplDef.PrjRefs.push(rwtestDef);
    
    var testDefs = testTplDef.createProjectDefsFromFolder(
        "%SRCDIR%\\tests",
        new RegExp("^.+\\.(?:cpp)$", "i"),
        new RegExp("^(?:\\.svn|Release.*|Debug.*|in|out|CVS|src|include)$","i"),
        new RegExp("^(?:rwstdmessages.cpp)$","i"), false);

    testArray = testArray.concat(testDefs);
    
///////////////////////////////////////////////////////////////////////////////
    var alltestsDef = new ProjectDef(".stdcxx_tests", typeGeneric);
    alltestsDef.VCProjDir = ProjectsDir + "\\tests";
    alltestsDef.OutDir = "$(SolutionDir)%CONFIG%\\tests";
    alltestsDef.IntDir = alltestsDef.OutDir;
    alltestsDef.PrjDeps = testDefs;

    testArray.push(alltestsDef);

    projectDefs.push(testArray);

///////////////////////////////////////////////////////////////////////////////
    var runtestsDef = new ProjectDef(".stdcxx_runtests", typeGeneric);
    runtestsDef.VCProjDir = ProjectsDir + "\\tests";
    runtestsDef.FilterDefs.push(
        new FilterDef("Script Files", null, ".js;.wsf", eFileTypeScript, false).
            addFiles("%SRCDIR%\\etc\\config\\windows",
                new Array("runall.wsf", "config.js", "utilities.js",
                          "summary.js")));
    runtestsDef.OutDir = "$(SolutionDir)%CONFIG%\\tests";
    runtestsDef.IntDir = runtestsDef.OutDir;
    runtestsDef.PreBuildCmd =
        "if exist \"$(OutDir)\\summary.htm\" del \"$(OutDir)\\summary.htm\"\r\n" +
        "if exist \"$(OutDir)\\runtests.log\" del \"$(OutDir)\\runtests.log\"";
    runtestsDef.CustomBuildFile = "runall.wsf";
    runtestsDef.CustomBuildCmd =
        "set PATH=$(SolutionDir)%CONFIG%\\bin;$(SolutionDir)%CONFIG%\\lib;%PATH%\r\n" +
        "set TOPDIR=%SRCDIR%\r\n" +
        "set BINDIR=$(SolutionDir)%CONFIG%\\bin\r\n" +
        "cscript /nologo \"%CUSTOMFILE%\"" +
        " /EXEDIR:\"$(OutDir)\"" +
        " /PRJDIR:\"" + runtestsDef.VCProjDir + "\"" +
        " /CONFIG:\"%SOLUTION%\"" +
        " /LOGFILE:\"runtests.log\"" +
        " /RUNFLAGS:\"--compat -x \'--no-popups --compat -O -\' -t " + EXEC_TIMEOUT + "\"";
    runtestsDef.CustomBuildOut = "$(OutDir)\\runtests.log";
    runtestsDef.CustomBuildDeps = "%FILES%";
    //runtestsDef.PrjDeps.push(alltestsDef);
    runtestsDef.PrjDeps.push(utilsDef);

    projectDefs.push(new Array(runtestsDef));

///////////////////////////////////////////////////////////////////////////////
    var localeArray = new Array();

    if (buildLocales)
    {
        var localeTplDef = new ProjectDef(null, typeGeneric);
        localeTplDef.VCProjDir = ProjectsDir + "\\locales";
        localeTplDef.FilterDefs.push(
            new FilterDef("Script Files", null, "js;wsf", eFileTypeScript, false).
                addFiles("%SRCDIR%\\etc\\config\\windows",
                         new Array("run_locale_utils.wsf")));
        localeTplDef.OutDir = "$(SolutionDir)nls";
        localeTplDef.IntDir = localeTplDef.OutDir + "\\Build\\$(ProjectName)";
        localeTplDef.CustomBuildFile = "run_locale_utils.wsf";
        localeTplDef.CustomBuildDeps = "%FILES%";
        localeTplDef.PrjDeps.push(localedefDef);

        var localeDefs = localeTplDef.createLocaleDefs("%SRCDIR%\\etc\\nls");

        localeArray = localeArray.concat(localeDefs);
    
        var localesDef = new ProjectDef(".stdcxx_locales", typeGeneric);
        localesDef.VCProjDir = ProjectsDir + "\\locales";
        localesDef.OutDir = "$(SolutionDir)nls";
        localesDef.IntDir = localesDef.OutDir;
        localesDef.PrjDeps = localeDefs;

        localeArray.push(localesDef);
    }

    projectDefs.push(localeArray);

///////////////////////////////////////////////////////////////////////////////
    var testlocaleArray = new Array();

    var testlocaleTplDef = new ProjectDef(".stdcxx_testlocales", typeGeneric);
    testlocaleTplDef.VCProjDir = ProjectsDir + "\\locales";
    testlocaleTplDef.FilterDefs.push(
        new FilterDef("Script Files", null, "js;wsf", eFileTypeScript, false).
            addFiles("%SRCDIR%\\etc\\config\\windows",
                     new Array("run_locale_utils.wsf")));
    testlocaleTplDef.OutDir = binPath;
    testlocaleTplDef.IntDir = testlocaleTplDef.OutDir;
    testlocaleTplDef.CustomBuildFile = "run_locale_utils.wsf";
    testlocaleTplDef.CustomBuildDeps = "%FILES%";
    testlocaleTplDef.PrjDeps.push(execDef);
    testlocaleTplDef.PrjDeps.push(localeDef);
    testlocaleTplDef.PrjDeps.push(localedefDef);

    if (testLocales)
    {
        var testlocaleDefs = testlocaleTplDef.createTestLocaleDefs("%SRCDIR%\\etc\\nls");
    
        testlocaleArray = testlocaleArray.concat(testlocaleDefs);
    }
    
    testlocaleTplDef.FilterDefs = new Array();
    testlocaleTplDef.FilterDefs.push(
        new FilterDef("Script Files", null, ".js;.wsf", eFileTypeScript, false).
            addFiles("%SRCDIR%\\etc\\config\\windows",
                new Array("runall.wsf", "config.js", "utilities.js",
                          "summary.js")));
    testlocaleTplDef.CustomBuildFile = "runall.wsf";
    testlocaleTplDef.CustomBuildCmd =
        "set PATH=$(SolutionDir)%CONFIG%\\bin;$(SolutionDir)%CONFIG%\\lib;%PATH%\r\n" +
        "cscript /nologo \"%CUSTOMFILE%\"" +
        " /EXEDIR:\"$(OutDir)\"" +
        " /CONFIG:\"%SOLUTION%\"" +
        " /LOGFILE:\"runloctests.log\"" +
        " /EXT:bat" +
        " /RUNFLAGS:\"-t " + EXEC_TIMEOUT + "\"";
    testlocaleTplDef.CustomBuildOut = "$(OutDir)\\runloctests.log";

    var testlocalesDef = testlocaleTplDef.createTestLocalesDef("%SRCDIR%\\etc\\nls");
    testlocaleArray.push(testlocalesDef);

    projectDefs.push(testlocaleArray);

///////////////////////////////////////////////////////////////////////////////
    if (copyDll)
    {
        // if project type is application and
        //   if it depends on stdcxx project then
        //     copy libstdxx.dll to project output directory
        //   if it depends on rwtest project then
        //     copy rwtest.dll to project output directory
        for (var i = 0; i < projectDefs.length; ++i)
        {
            var projectArray = projectDefs[i];

            for (var j = 0; j < projectArray.length; ++j)
            {
                var projectDef = projectArray[j];

                if (projectDef.Type != typeApplication)
                    continue;
    
                var arrDeps = projectDef.PrjRefs.concat(projectDef.PrjDeps);
                var command = "";
    
                var cmdtpl = "set src=_SRC_\r\n" +
                             "set dst=_DST_\r\n" +
                             "if /I not %src%==%dst% (\r\n" +
                             "if exist %src% (\r\n" +
                             "del %dst%\r\n" +
                             "copy /Y %src% %dst%\r\n" +
                             "))";

                if (0 <= arrayIndexOf(arrDeps, stdcxxDef))
                {
                    var libname = "libstd%CONFIG%.dll";
                    var src = "\"" + libPath + "\\" + libname + "\"";
                    var dst = "\"$(OutDir)\\" + libname + "\"";
                    var cmd = cmdtpl.replace("_SRC_", src).replace("_DST_", dst);
                    if (0 == command.length)
                        command = cmd;
                    else
                        command += "\r\n" + cmd;
                }
    
                if (0 <= arrayIndexOf(arrDeps, rwtestDef))
                {
                    var libname = "rwtest.dll";
                    var src = "\"$(SolutionDir)%CONFIG%\\tests\\" + libname + "\"";
                    var dst = "\"$(OutDir)\\" + libname + "\"";
                    var cmd = cmdtpl.replace("_SRC_", src).replace("_DST_", dst);
                    if (0 == command.length)
                        command = cmd;
                    else
                        command += "\r\n" + cmd;
                }
    
                if (null == projectDef.PostBuildCmd || "" == projectDef.PostBuildCmd)
                    projectDef.PostBuildCmd = command;
                else
                    projectDef.PostBuildCmd = command + "\r\n" + projectDef.PostBuildCmd;
            }
        }
    }

    return projectDefs;
}

// create VCProject's using ProjectDed definitions
// projectDefs - array with project definitions
// report - callback function to report progress
function CreateProjects(projectDefs, report)
{
    for (var i = 0; i < projectDefs.length; ++i)
    {
        var projectArray = projectDefs[i];

        for (var j = 0; j < projectArray.length; ++j)
        {
            var projectDef = projectArray[j];
    
            // turn off RTTI support if project in NonRTTIProjects array
            if (0 <= arrayIndexOf(NonRTTIProjects, projectDef.Name))
                projectDef.RTTI = false;
    
            projectDef.createVCProject(VCProjectEngine, report);
        }
    }
}

// configure dependencies between projects
// (insert <References> section to the .vcproj file)
// projectDefs - array with project definitions
function ConfigureDependencies(projectDefs)
{
    for (var i = 0; i < projectDefs.length; ++i)
    {
        var projectArray = projectDefs[i];

        for (var j = 0; j < projectArray.length; ++j)
        {
            var projectDef = projectArray[j];

            var VCProject = projectDef.VSProject;
    
            var prjrefs = projectDef.PrjRefs;
            if (0 == prjrefs.length)
                continue;
    
            var file = fso.OpenTextFile(VCProject.ProjectFile, 1, false);
            var text = file.ReadAll();
            file.Close();
            var refs = "";
            for (var k = 0; k < prjrefs.length; ++k)
            {
                refs += "\t\t<ProjectReference\n";
                refs += "\t\t\tReferencedProjectIdentifier=\"" +
                        prjrefs[k].VSProject.ProjectGUID + "\"\n";
                refs += "\t\t/>\n";
            }
            var pos = text.indexOf("\t</References>");
            if (0 > pos)
            {
                var str = "\t</Configurations>";
                pos = text.indexOf(str);
                if (0 <= pos)
                {
                    refs = "\n\t<References>\n" + refs + "\t</References>";
                    pos += str.length;
                }
            }
            text = text.substr(0, pos) + refs + text.substr(pos);
            text.replace("\t</References>", refs);
            file = fso.CreateTextFile(VCProject.ProjectFile, true, false);
            file.Write(text);
            file.Close();
        }
    }
}
