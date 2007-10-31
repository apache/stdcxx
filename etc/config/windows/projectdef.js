//
// $Id$
//
// projectdef.js - ProjectDef class definition
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

var TristateUseDefault = -2;
var TristateTrue = -1;
var TristateFalse = 0;

var dte = null;
var VCProjectEngine = null;
var ICConvertTool = "ICProjConvert90.exe";

// test availability of utility file
// returns true if utility is available
// otherwise returns false 
function TestUtil(cmd)
{
    try
    {
        return (0 == WshShell.Run(cmd + " /?", 0, true));
    }
    catch (e) {}
    
    return false;
}

// init VisualStudio objects using specified configuration
// config - name of configuration (e.g. msvc-7.0)
// freedte - if undefined or true then free dte inside function
function InitVSObjects(config, freedte)
{
    getCompilerOpts(config);

    try
    {
        dte = WScript.CreateObject("VisualStudio.DTE." + VERSION);
        if (DEVENV == "")
        {
            // use devenv.com executable, if it present
            var comName = dte.FullName.replace(/\.exe$/i, ".com");
            DEVENV = fso.FileExists(comName) ? comName : dte.FullName;
        }
    }
    catch (e) {}

    if (CONVERT)
    {
        if (ICCCONVERT == "")
            ICCCONVERT = "ICProjConvert90.exe";

        var ICConvertTool = ICCCONVERT;

        var arrPath = new Array(ICConvertTool);

        if (0 > ICConvertTool.indexOf("\\"))
        {
            // no file path specified
            // try to detect the path
            if (null != dte)
            {
                try
                {
                    var isettings = dte.GetObject("IntelOptions");
                    for (var i = 1; i <= isettings.CompilersCount; ++i)
                    {
                        var paths = isettings.Compiler(i).ExecutablePath.split(";");
                        for (var j = 0; j < paths.length; ++j)
                            arrPath.push("\"" + isettings.Evaluate(paths[j]) + "\\" + ICConvertTool + "\"");
                    }
                    isettings = null;
                }
                catch (e) {}
            }
        }

        var success = false;
        
        for (var i = 0; i < arrPath.length; ++i)
            if (TestUtil(arrPath[i]))
            {
                success = true;
                ICCCONVERT = arrPath[i];
                break;
            }
            
        if (!success)
        {
            WScript.StdErr.WriteLine(ICCCONVERT + " conversion utility not found");
            WScript.StdErr.WriteLine("You should check ICCCONVERT configuration variable");
            WScript.Quit(3);
        }
    }

    if (("undefined" == typeof(freedte) || true == freedte) && (null != dte))
    {
        dte.Quit();
        dte = null;
    }

    try
    {
        VCProjectEngine = WScript.CreateObject("VisualStudio.VCProjectEngine." + VERSION);
    }
    catch (e)
    {
        WScript.StdErr.WriteLine("Unable to create VCProjectEngine object: " + e.message);
        return false;
    }

    return true;
}

//------------------------------------------------
// ProjectDef class
//------------------------------------------------

// ProjectDef .ctor
function ProjectDef(name, type)
{
    this.Name = name;
    this.Type = type;
    this.SubSystem = typeGeneric == type ? null :
        (typeApplication == type ? subSystemConsole : subSystemWindows);
    this.RTTI = true;
    this.VCProjDir = "%BUILDDIR%";
    this.FilterDefs = new Array();
    this.Defines = null;
    this.Includes = null;
    this.PrepOpts = null
    this.CppOpts = null;
    this.LnkOpts = null;
    this.LibOpts = null;
    this.OutDir = null;
    this.IntDir = null;
    this.Libs = null;
    this.OutFile = null;
    this.CustomBuildFile = null;
    this.CustomBuildDesc = null;
    this.CustomBuildCmd = null;
    this.CustomBuildOut = null;
    this.CustomBuildDeps = null;
    this.PreLinkDesc = null;
    this.PreLinkCmd = null;
    this.PreBuildDesc = null;
    this.PreBuildCmd = null;
    this.PostBuildDesc = null;
    this.PostBuildCmd = null;
    this.VSProject = null;
    this.PrjRefs = new Array();
    this.PrjDeps = new Array();
}

ProjectDef.prototype.clone = projectCloneDef;
ProjectDef.prototype.createVCProject = projectCreateVCProject;
ProjectDef.prototype.createProjectDefsFromFolder = projectCreateProjectDefsFromFolder;
ProjectDef.prototype.createLocaleDefs = projectCreateLocaleDefs;
ProjectDef.prototype.createTestLocaleDefs = projectCreateTestLocaleDefs;
ProjectDef.prototype.createTestLocalesDef = projectCreateTestLocalesDef;

// returns copy of ProjectDef object
function projectCloneDef()
{
    var clone = new ProjectDef(this.Name, this.Type);

    clone.SubSystem = this.SubSystem;
    clone.RTTI = this.RTTI;
    clone.VCProjDir = this.VCProjDir;
    clone.FilterDefs = this.FilterDefs.concat(new Array());
    clone.Defines = this.Defines;
    clone.Includes = this.Includes;
    clone.PrepOpts = this.PrepOpts;
    clone.CppOpts = this.CppOpts;
    clone.LnkOpts = this.LnkOpts;
    clone.LibOpts = this.LibOpts;
    clone.OutDir = this.OutDir;
    clone.IntDir = this.IntDir;
    clone.Libs = this.Libs;
    clone.OutFile = this.OutFile;
    clone.CustomBuildFile = this.CustomBuildFile;
    clone.CustomBuildDesc = this.CustomBuildDesc;
    clone.CustomBuildCmd = this.CustomBuildCmd;
    clone.CustomBuildOut = this.CustomBuildOut;
    clone.CustomBuildDeps = this.CustomBuildDeps;
    clone.PreLinkDesc = this.PreLinkDesc;
    clone.PreLinkCmd = this.PreLinkCmd;
    clone.PreBuildDesc = this.PreBuildDesc;
    clone.PreBuildCmd = this.PreBuildCmd;
    clone.PostBuildDesc = this.PostBuildDesc;
    clone.PostBuildCmd = this.PostBuildCmd;
    clone.VSProject = this.VSProject;
    clone.PrjRefs = this.PrjRefs.concat(new Array());
    clone.PrjDeps = this.PrjDeps.concat(new Array());

   return clone;
}

// preprocess defines using info about selected configuration
// defines - string with compiler defines, separated by ';'
//           the define syntax can contain ?: construction
//           supported conditions: debug, dll, mt
//           i.e. "debug?_RWSTD_DEBUG:_RWSTD_RELEASE"
//           will be replaced to "_RWSTD_DEBUG" in debug configurations
//           and to "_RWSTD_RELEASE" in non-debug configurations
// confInfo - configuration info object
function processDefines(defines, confInfo)
{
    var arr = defines.split(";");
    var res = new Array();
    for (var i = 0; i < arr.length; ++i)
    {
        var str = arr[i];
        var pos1 = str.indexOf("?");
        if (pos1 > 0)
        {
            var pos2 = str.indexOf(":", pos1 + 1);
            if (pos2 < 0)
                pos2 = str.length;
            var s1 = str.substring(0, pos1);
            var s2 = str.substring(pos1 + 1, pos2);
            var s3 = pos2 < str.length ? str.substr(pos2 + 1) : "";
            
            switch (s1)
            {
            case "debug":
                str = confInfo.debug ? s2 : s3;
                break;
            case "dll":
                str = confInfo.dll ? s2 : s3;
                break;
            case "mt":
                str = confInfo.mt ? s2 : s3;
                break;
            }
        }
        if (str.length > 0)
            res.push(str);
    }
    return res.join(";");
}

// returns string with list of files in VCFiles of VCFilters collection
// files - source VCFiles or VCFilters collection
// delim - delimiter
function combFiles(files, delim)
{
    var ret = "";
    for (var i = 1; i <= files.Count; ++i)
    {
        if (ret != "")
            ret += delim;
        ret += files.Item(i).RelativePath;
    }
    return ret;
}

// returns string with list of files in VCFilter object
// filter - source VCFilter object
// delim - delimiter
function combFilter(filter, delim)
{
    var ret = combFiles(filter.Files, delim);
    if (ret != "")
        ret += delim;
    ret += combFilters(filter.Filters, delim);
    return ret;
}

// returns string with list of files in VCFilters collection
// filters - VCFilters collection
// delim - delimiter
function combFilters(filters, delim)
{
    var ret = "";
    for (var i = 1; i <= filters.Count; ++i)
    {
        if (ret != "")
            ret += delim;
        ret += combFilter(filters.Item(i), delim);
    }
    return ret;
}

// assign value to a property if it's exists
function setProperty(property, value)
{
    if ("undefined" != typeof(property))
    {
        property = value;
        return true;
    }

    return false;
}

// create VCProject object from ProjectDef
// engine - VCProjectEngine object
// report - callback function to report progress
function projectCreateVCProject(engine, report)
{
    if (typeof(report) != "undefined" && null != report)
        report("  Creating " + this.Name + "...");
    
    var PrjName = removeLeadingDot(this.Name);
    
    var prjMacros = cmnMacros.concat();
    prjMacros.push(new Macro("%PRJNAME%", PrjName));
        
    var VCProject = engine.CreateProject(this.Name);
    
    VCProject.Name = this.Name;
    var PrjDir = ReplaceMacros(this.VCProjDir, prjMacros);
    var PrjFile = PrjDir + "\\" + PrjName + ".vcproj";
    VCProject.ProjectFile = PrjFile;
    VCProject.ProjectGUID = createUUID();
    
    VCProject.AddPlatform(PLATFORM);

    var FixedPrjName = PrjName;
    var dotObj = ".obj";

    var VC7xWknd = 0 <= PrjName.indexOf(dotObj)
                && ("7" == VERSION || "7.1" == VERSION);

    var PostBuildCmd = this.PostBuildCmd;

    if (VC7xWknd)
    {
        FixedPrjName = FixedPrjName.replace(new RegExp(dotObj, "ig"), "_obj");
        if (null != PostBuildCmd)
            PostBuildCmd += "\r\n";
        else
            PostBuildCmd = "";

        var srcDir = "$(OutDir)\\" + FixedPrjName;
        var dstDir = "$(OutDir)\\" + PrjName;
        PostBuildCmd += "md \"" + dstDir + "\"\r\n" +
            "copy \"" + srcDir + "\\" + FixedPrjName + dotObj + "\" \"" + dstDir + "\\" + PrjName + dotObj + "\"\r\n" +
            "copy \"" + srcDir + "\\buildlog.htm\" \"" + dstDir + "\"";
    }
    
    for (var i = 0; i < confNames.length; ++i)
        VCProject.AddConfiguration(confNames[i]);
        
    var OutDir = this.OutDir != null ?
        ReplaceMacros(this.OutDir, prjMacros) : "%CONFIG%";
    var IntDir = this.IntDir != null ?
        ReplaceMacros(this.IntDir, prjMacros) : OutDir + "\\" + FixedPrjName;

    // add files
    for (var i = 0; i < this.FilterDefs.length; ++i)
        this.FilterDefs[i].createVCFilter(VCProject);

    prjMacros.push(new Macro("%FILES%", combFiles(VCProject.Files, ";")));
    
    var VCCustomFile = null;
    if (null != this.CustomBuildFile)
        VCCustomFile = VCProject.Files(this.CustomBuildFile);
    
    // set common configuration settings
    for (var i = 1; i <= VCProject.Configurations.Count; ++i)
    {
        var conf = VCProject.Configurations.Item(i);
        var confInfo = configs.get(conf.ConfigurationName);

        var cfgMacros = new Array(
                            new Macro("%CONFIG%", confInfo.out),
                            new Macro("%CFGNAME%", conf.ConfigurationName));

        conf.CharacterSet = charSetMBCS;
        conf.IntermediateDirectory = ReplaceMacros(IntDir, cfgMacros);
        conf.OutputDirectory = ReplaceMacros(OutDir, cfgMacros);
        conf.ConfigurationType = (typeLibrary != this.Type) ?
            this.Type :
            (confInfo.dll ? typeDynamicLibrary : typeStaticLibrary);

        var ext = "";
        switch (conf.ConfigurationType)
        {
        case typeApplication:
            ext = ".exe";
            break;
        case typeDynamicLibrary:
            ext = ".dll";
            break;
        case typeStaticLibrary:
            ext = ".lib";
            break;
        }
        cfgMacros.push(new Macro("%EXT%", ext));

        var allMacros = prjMacros.concat(cfgMacros);

        var OutFile = this.OutFile != null ?
            ReplaceMacros(this.OutFile, allMacros) :
            "$(OutDir)\\" + PrjName + ext;

        var IngoreLibs = "";
        if (confInfo.dll)
        {
            IngoreLibs = confInfo.debug ? "msvcprtd.lib" : "msvcprt.lib";
        }
        else
        {
            if (confInfo.mt || NOSTCRT)
                IngoreLibs = confInfo.debug ? "libcpmtd.lib" : "libcpmt.lib";
            else
                IngoreLibs = confInfo.debug ? "libcpd.lib" : "libcp.lib";
        }

        var compiler = conf.Tools.Item("VCCLCompilerTool");
        if (null != compiler)
        {
            if (null != this.Includes)
                compiler.AdditionalIncludeDirectories =
                    ReplaceMacros(this.Includes, allMacros);

            compiler.AdditionalOptions = CPPFLAGS + " " +
                (null != this.CppOpts ? this.CppOpts : "");
            if (null != this.PrepOpts)
                compiler.GeneratePreprocessedFile = this.PrepOpts;
            compiler.DebugInformationFormat = debugEnabled;

            if (typeStaticLibrary == conf.ConfigurationType)
            {
                // generate the source pdb in the OutDir
                compiler.ProgramDataBaseFileName = changeFileExt(OutFile, "pdb");
            }

            compiler.SuppressStartupBanner = true;
            compiler.WarningLevel = warningLevel_3;
            setProperty(compiler.Detect64BitPortabilityProblems, false);
            
            if (null != this.Defines)
                compiler.PreprocessorDefinitions = processDefines(this.Defines, confInfo);
            
            if (confInfo.debug)
            {
                compiler.Optimization = optimizeDisabled;
                compiler.MinimalRebuild = true;
                //setProperty(compiler.SmallerTypeCheck, true);
                setProperty(compiler.BasicRuntimeChecks, runtimeBasicCheckAll);
                compiler.BufferSecurityCheck = true;
            }
            else
            {
                if (typeApplication == conf.ConfigurationType)
                {
                    if (!setProperty(compiler.OptimizeForWindowsApplication, true))
                        compiler.AdditionalOptions += " /GA";
                }
                compiler.Optimization = optimizeMaxSpeed;
                compiler.MinimalRebuild = false;
                setProperty(compiler.SmallerTypeCheck, false);
                setProperty(compiler.BasicRuntimeChecks, runtimeBasicCheckNone);
                compiler.BufferSecurityCheck = false;
            }
            
            compiler.ExceptionHandling = cppExceptionHandlingYes;
            compiler.RuntimeTypeInfo = this.RTTI;

            if (confInfo.dll)
            {
                // the singlethreaded dll runtimes are not present
                // always use the multithreaded dll runtime
                compiler.RuntimeLibrary = confInfo.debug ?
                    rtMultiThreadedDebugDLL : rtMultiThreadedDLL;
            }
            else
            {
                if (confInfo.mt || NOSTCRT)
                    // use multithreaded runtimes
                    compiler.RuntimeLibrary = confInfo.debug ?
                        rtMultiThreadedDebug : rtMultiThreaded;
                else
                    // use singlethreaded runtimes
                    compiler.RuntimeLibrary = confInfo.debug ?
                        rtSingleThreadedDebug : rtSingleThreaded;
            }

            compiler.UsePrecompiledHeader = pchNone;

            if (VC7xWknd)
                compiler.ObjectFile = "$(IntDir)/" + FixedPrjName + dotObj;
        }

        var linker = conf.Tools.Item("VCLinkerTool");
        if (null != linker)
        {
            linker.AdditionalOptions = LDFLAGS + " " +
                (null != this.LnkOpts ? this.LnkOpts : "");

            if (null != this.Libs)
                linker.AdditionalDependencies = this.Libs;

            linker.LinkIncremental = linkIncrementalNo;
            linker.SuppressStartupBanner = true;
            linker.GenerateDebugInformation = true;
            linker.ProgramDatabaseFile = changeFileExt(OutFile, "pdb");
            linker.IgnoreDefaultLibraryNames = IngoreLibs;
            linker.SubSystem = this.SubSystem;

            if (confInfo.debug)
            {
                linker.OptimizeReferences = optReferencesDefault;
                linker.EnableCOMDATFolding = optFoldingDefault;
            }
            else
            {
                linker.OptimizeReferences = optReferences;
                linker.EnableCOMDATFolding = optFolding;
            }

            linker.OutputFile = OutFile;

            if (this.Type != typeApplication)
                linker.ImportLibrary = changeFileExt(linker.OutputFile, "lib");
        }

        var librarian = conf.Tools.Item("VCLibrarianTool");
        if (null != librarian)
        {
            if (null != this.LibOpts)
                linker.AdditionalOptions = this.LibOpts;
                
            librarian.SuppressStartupBanner = true;
            librarian.IgnoreDefaultLibraryNames = IngoreLibs;
            librarian.OutputFile = OutFile;
        }

        if (null != this.PreLinkCmd)
        {
            var tool = conf.Tools.Item("VCPreLinkEventTool");
            tool.CommandLine = ReplaceMacros(this.PreLinkCmd, allMacros);
            if (null != this.PreLinkDesc)
                tool.Description = ReplaceMacros(this.PreLinkDesc, allMacros);
        }

        if (null != this.PreBuildCmd)
        {
            var tool = conf.Tools.Item("VCPreBuildEventTool");
            tool.CommandLine = ReplaceMacros(this.PreBuildCmd, allMacros);
            if (null != this.PreBuildDesc)
                tool.Description = ReplaceMacros(this.PreBuildDesc, allMacros);
        }

        if (null != PostBuildCmd)
        {
            var tool = conf.Tools.Item("VCPostBuildEventTool");
            tool.CommandLine = ReplaceMacros(PostBuildCmd, allMacros);
            if (null != this.PostBuildDesc)
                tool.Description = ReplaceMacros(this.PostBuildDesc, allMacros);
        }

        if (null != this.CustomBuildCmd)
        {
            var tool = (null == VCCustomFile) ? conf.Tools.Item("VCCustomBuildTool") :
                VCCustomFile.FileConfigurations.Item(conf.ConfigurationName).Tool;
            
            var cmd = ReplaceMacros(this.CustomBuildCmd, allMacros);
            if (null != VCCustomFile)
                cmd = cmd.replace(/(%CUSTOMFILE%)/g, VCCustomFile.FullPath);
            tool.CommandLine = cmd;
            tool.Outputs = null != this.CustomBuildOut ?
                ReplaceMacros(this.CustomBuildOut, allMacros) : "";
            if (null != this.PostBuildDesc)
                tool.Description = ReplaceMacros(this.CustomBuildDesc, allMacros);
            if (null != this.CustomBuildDeps)
                tool.AdditionalDependencies = ReplaceMacros(this.CustomBuildDeps, allMacros);
        }
    }
    
    if (!fso.FolderExists(PrjDir))
        fso.CreateFolder(PrjDir);
    
    VCProject.Save();
    
    this.VSProject = VCProject;
}

// create array of ProjectDef objects (one object for each file)
// startDir - start folder
// inclFiles - regular expression to define include files
// exclDirs - regular expression to define exclude folder
// exclFiles - regular expression to define exclude files
// shiftOutDir - if true then add subfolder name to the OutDir path
function projectCreateProjectDefsFromFolder(startDir,
    inclFiles, exclDirs, exclFiles, shiftOutDir)
{
    var projectDefs = new Array();
    
    var folder = fso.GetFolder(ReplaceMacros(startDir, cmnMacros));
        
    var enumSubFolders = new Enumerator(folder.SubFolders);
    for (; !enumSubFolders.atEnd(); enumSubFolders.moveNext())
    {
        var subFolder = enumSubFolders.item();
        if (exclDirs.test(subFolder.Name))
        {
            // skip excluded folder
            continue;
        }
        
        var newDefs = this.createProjectDefsFromFolder(subFolder.Path,
            inclFiles, exclDirs, exclFiles, shiftOutDir);

        if (shiftOutDir)
        {
            for (var i = 0; i < newDefs.length; ++i)
                newDefs[i].OutDir += "\\" + subFolder.Name;
        }
        
        projectDefs = projectDefs.concat(newDefs);
    }

    var enumFiles = new Enumerator(folder.Files);
    for (; !enumFiles.atEnd(); enumFiles.moveNext())
    {
        var file = enumFiles.item();
        if (inclFiles.test(file.Name))
        {
            if (exclFiles.test(file.Name))  // we should exclude this file
                continue;
            
            var lastPoint = file.Name.lastIndexOf(".");
            var prjName = file.Name.substr(0, lastPoint);
            
            var projectDef = this.clone();
            projectDef.Name = prjName;
            projectDef.FilterDefs.push(
                new FilterDef(sourceFilterName, sourceFilterUuid,
                              "cpp;c;cxx;s;cc", eFileTypeCppCode, false).
                addFiles(null, new Array(file.Path)));
            
            projectDefs.push(projectDef);
        }   
    }
    
    return projectDefs;
}

//------------------------------------------------
// Locale class
//------------------------------------------------

// Locale .ctor
function Locale(name, cmname, srcname)
{
    this.Name = name;
    this.cmName = cmname;
    this.srcName = srcname;
}

// returns array of Locale objects parsing the gen_list file
// nlsDir - folder containing file "gen_list"
function initLocalesList(nlsDir)
{
    var listFileName = nlsDir + "\\gen_list";
    if (!fso.FileExists(listFileName))
    {
        WScript.StdErr.WriteLine("Generate: Fatal error: "
            + "File "+ listFileName + " does not exist");
        WScript.Quit(3);
    }
    
    var ForReading = 1;
    var stream = fso.OpenTextFile(listFileName, ForReading);
    if (!stream)
    {
        WScript.StdErr.WriteLine("Generate: Fatal error: "
            + "Cannot open file "+ listFileName);
        WScript.Quit(3);
    }

    var arrLocales = new Array();
    
    while (!stream.AtEndOfStream)
    {
        var line = stream.ReadLine();

        var name = line.replace(new RegExp("^\([^ ]*\) *\([^ ]*\)"),
                                "$1\.$2")
                       .replace(new RegExp("\([^.]*\)\(.euro\)\([^ ]*\)"),
                                "$1$3@euro")
                       .replace(new RegExp("\([^.]*\)\(.cyrillic\)\([^ ]*\)"),
                                "$1$3@cyrillic");
        var pos = name.indexOf(" ");
        if (0 <= pos)
            name = name.substr(0, pos);

        var srcname = name.replace(new RegExp("\([^.]*\)\.\([^@]*\)\(.*\)"),
                                 "$1$3")
                        .replace("@", ".");

        var cmname = name.replace(new RegExp("\([^.]*\)\.\([^@]*\)\(.*\)"),
                                 "$2");

        arrLocales.push(new Locale(name, cmname, srcname));
    }
    
    return arrLocales;
}

// create array of ProjectDef objects for build locales
// (one object for each locale)
// nlsDir - folder containing locale source files
function projectCreateLocaleDefs(nlsDir)
{
    nlsDir = ReplaceMacros(nlsDir, cmnMacros);

    if (typeof(this.arrLocales) == "undefined")
        ProjectDef.prototype.arrLocales = initLocalesList(nlsDir);
        
    var projectDefs = new Array();

    for (var i = 0; i < this.arrLocales.length; ++i)
    {
        var locale = this.arrLocales[i];
                        
        srcFileName = nlsDir + "\\src\\" + locale.srcName;
        cmFileName = nlsDir + "\\charmaps\\" + locale.cmName;

        var cmFile;
        var srcFile;
        
        try
        {
            cmFile = fso.GetFile(cmFileName);
        }
        catch (e)
        {
            WScript.StdErr.WriteLine("Generate: Fatal error: File " +
                cmFileName + " not found");
            WScript.Quit(3);
        }

        try
        {
            srcFile = fso.GetFile(srcFileName);
        }
        catch (e)
        {
            WScript.StdErr.WriteLine("Generate: Fatal error: File " +
                srcFileName + " not found");
            WScript.Quit(3);
        }

        var projectDef = this.clone();
        projectDef.Name = locale.Name;
        projectDef.FilterDefs.push(
            new FilterDef("Charmap", null, "cm", eFileTypeText, false).
            addFiles(null, new Array(cmFile.Path)));
        projectDef.FilterDefs.push(
            new FilterDef("Src", null, "", eFileTypeText, false).
            addFiles(null, new Array(srcFile.Path)));
        projectDef.CustomBuildCmd =
            "set PATH=$(SolutionDir)%CONFIG%\\lib;%PATH%\r\n" +
            "\"$(SolutionDir)%CONFIG%\\bin\\localedef.exe\" -w -c" +
            " -f \"" + cmFileName + "\"" + " -i \"" + srcFileName + "\"" +
            " \"$(OutDir)\\" + locale.Name + "\"";
        projectDef.CustomBuildOut = "$(OutDir)\\" + locale.Name;
        
        projectDefs.push(projectDef);
    }    

    return projectDefs;
}

// create array of ProjectDef objects for test locales
// (one object for each locale)
// nlsDir - folder containing locale source files
function projectCreateTestLocaleDefs(nlsDir)
{
    nlsDir = ReplaceMacros(nlsDir, cmnMacros);

    if (typeof(this.arrLocales) == "undefined")
        ProjectDef.prototype.arrLocales = initLocalesList(nlsDir);
        
    var projectDefs = new Array();

    var srcdir = "%SRCDIR%\\etc\\config\\windows";
    var bindir = "$(SolutionDir)%CONFIG%\\bin";
    var exec = bindir + "\\exec.exe";
    var test = bindir + "\\sanity_test";

    var setPath = "set PATH=$(SolutionDir)%CONFIG%\\lib;%PATH%";

    // create test_locale_sanity project
    var sanityDef = this.clone();
    sanityDef.Name = "test_locale_sanity";
    if (null == sanityDef.PreBuildCmd)
        sanityDef.PreBuildCmd = "";
    else
        sanityDef.PreBuildCmd += "\r\n";
    sanityDef.PreBuildCmd +=
        "echo cscript /nologo \"" + srcdir + "\\run_locale_utils.wsf\"" +
        " /s /b:\"" + bindir + "\" > \"" + test + ".bat\"";
    sanityDef.CustomBuildCmd = setPath + "\r\n\"" + exec + "\" -t " + EXEC_TIMEOUT + " \"" + test + ".bat\"";
    sanityDef.CustomBuildOut = test + ".out";
    projectDefs.push(sanityDef);
        
    for (var i = 0; i < this.arrLocales.length; ++i)
    {
        var locale = this.arrLocales[i];
        srcFileName = nlsDir + "\\src\\" + locale.srcName;
        cmFileName = nlsDir + "\\charmaps\\" + locale.cmName;

        var cmFile;
        var srcFile;
        
        try
        {
            cmFile = fso.GetFile(cmFileName);
        }
        catch (e)
        {
            WScript.StdErr.WriteLine("Generate: Fatal error: File " +
                cmFileName + " not found");
            WScript.Quit(3);
        }

        try
        {
            srcFile = fso.GetFile(srcFileName);
        }
        catch (e)
        {
            WScript.StdErr.WriteLine("Generate: Fatal error: File " +
                srcFileName + " not found");
            WScript.Quit(3);
        }

        test = bindir + "\\" + locale.Name;

        var projectDef = this.clone();
        projectDef.Name = "test_" + locale.Name;
        projectDef.FilterDefs.push(
            new FilterDef("Charmap", null, "cm", eFileTypeText, false).
            addFiles(null, new Array(cmFile.Path)));
        projectDef.FilterDefs.push(
            new FilterDef("Src", null, "", eFileTypeText, false).
            addFiles(null, new Array(srcFile.Path)));
        if (null == projectDef.PreBuildCmd)
            projectDef.PreBuildCmd = "";
        else
            projectDef.PreBuildCmd += "\r\n";
        projectDef.PreBuildCmd +=
            "echo cscript /nologo \"" + srcdir + "\\run_locale_utils.wsf\"" +
            " /f /b:\"" + bindir + "\" /i:\"" + nlsDir + "\"" +
            " /l:" + locale.Name + " > \"" + test + ".bat\"";
        projectDef.CustomBuildCmd = setPath + "\r\n\"" + exec + "\" -t " + EXEC_TIMEOUT + " \"" + test + ".bat\"";
        projectDef.CustomBuildOut = test + ".out";
        projectDef.PrjDeps.push(sanityDef);
        
        projectDefs.push(projectDef);
    }    

    return projectDefs;
}

// create  ProjectDef object for test all locales
// nlsDir - folder containing locale source files
function projectCreateTestLocalesDef(nlsDir)
{
    nlsDir = ReplaceMacros(nlsDir, cmnMacros);

    if (typeof(this.arrLocales) == "undefined")
        ProjectDef.prototype.arrLocales = initLocalesList(nlsDir);
        
    var bindir = "$(SolutionDir)%CONFIG%\\bin";
    var test = "sanity_test";

    // create test_locale_sanity project
    var projectDef = this.clone();
    if (null == projectDef.PreBuildCmd)
    {
        projectDef.PreBuildCmd =
            "set soldir=%BUILDDIR%\r\n" +
            "set bindir=%soldir%\\%CONFIG%\\bin";
    }

    projectDef.PreBuildCmd += "\r\n" +
        "set etcdir=%SRCDIR%\\etc\r\n" +
        "set util=\"%etcdir%\\config\\windows\\run_locale_utils.wsf\"\r\n";

    projectDef.PreBuildCmd +=
        "echo cscript /nologo %util% /s /b:\"%bindir%\" > \"%bindir%\\" + test + ".bat\"";

    var arrLocs = new Array();
    var locales = "";
        
    for (var i = 0; i < this.arrLocales.length; ++i)
    {
        var locale = this.arrLocales[i];
        srcFileName = nlsDir + "\\src\\" + locale.srcName;
        cmFileName = nlsDir + "\\charmaps\\" + locale.cmName;

        var cmFile;
        var srcFile;
        
        try
        {
            cmFile = fso.GetFile(cmFileName);
        }
        catch (e)
        {
            WScript.StdErr.WriteLine("Generate: Fatal error: File " +
                cmFileName + " not found");
            WScript.Quit(3);
        }

        try
        {
            srcFile = fso.GetFile(srcFileName);
        }
        catch (e)
        {
            WScript.StdErr.WriteLine("Generate: Fatal error: File " +
                srcFileName + " not found");
            WScript.Quit(3);
        }

        if (locales.length + locale.Name.length > 1012)
        {
            arrLocs.push(locales);
            locales = "";
        }
        else
        {
            if (0 < locales.length)
                locales += " ";
            locales += locale.Name;
        }
    }

    if (0 < locales.length)
        arrLocs.push(locales);

    for (var i = 0; i < arrLocs.length; ++i)
    {
        projectDef.PreBuildCmd += "\r\nset locales=" + arrLocs[i] + "\r\n" +
            "for %%l in (%locales%) do " +
            "echo cscript /nologo %util% /f /b:\"%bindir%\" " +
            "/i:\"%etcdir%\\nls\" /l:%%l > \"%bindir%\\%%l.bat\"";
    }

    return projectDef;
}
