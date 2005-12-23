//
// $Id$
//
// defines different utility functions

// returns parent folder for a path
function getParentFolder(path)
{
    var idx = path.lastIndexOf('\\');
    return path.substr(0, idx);
}

var rxUUID = /[a-f0-9\-]+/i;
// generates new UUID. uuidgen shall be runnable via PATH
function createUUID()
{
    var exec = WshShell.Exec("uuidgen");
    var time = 0;
    while (exec.Status == 0)
    {
        WScript.Sleep(100); // wait for completion
        time += 100;
        if (time > 5000)
        {
            WScript.StdErr.WriteLine(
                "createUUID: Fatal error: uuidgen" + 
                " failed to complete in 5" + 
                " seconds");
            WScript.Quit(3);
        }
    }
    var result = exec.ExitCode;
    if (result != 0)
    {
        WScript.StdErr.WriteLine(
            "createUUID: Fatal error: uuidgen"
                + " failed");				
        WScript.Quit(3);
    }
    var id = exec.StdOut.ReadAll();
    return "{" + rxUUID.exec(id)[0].toUpperCase() +"}";
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
        var cmd = "windiff -F" + flags + " " + tResPath;
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

function makeTempFile()
{
    var tfolder, TemporaryFolder = 2;
    tfolder = fso.GetSpecialFolder(TemporaryFolder);
    
    var tname = fso.GetTempName();
    var tfile = tfolder.CreateTextFile(tname);
    tfile.Close();
    
    return tfolder.Path + "\\" + tname;
}

function encodeHTML(srcString)
{
    var res = srcString;
    var srcS = new Array ( "&", "<", ">", "\"" );
    var destS = new Array ( "&amp;", "&lt;", "&gt;", "&quot;" );
    for (var t = 0; t < srcS.length; t++)
    {
        var reRep = new RegExp(srcS[t], "mg");
        res = res.replace(reRep, destS[t]);
    }
    
    return res;
}

function removeLeadingSymbol(dotName, symbol)
{
    var index = dotName.indexOf(symbol);
    if (0 != index)
        return dotName;
        
    var resName = dotName.substr(symbol.length);
    return resName;
}

function removeLeadingDot(dotName)
{
    var index = dotName.indexOf(".");
    if (0 != index)
        return dotName;
        
    var resName = dotName.substr(1);
    return resName;
}

// icc specific - solution conversion
function convertSolutionImpl(slnFileName, toICC, logStream)
{   
    var convertKey = toICC == true ? "/IC" : "/VC";
    var cmdICConvert = iccConversionUtility + " \"" + slnFileName + 
        "\" " + convertKey;
    
    var convertKeyFriendly = toICC == true ? "ICC" : "MSVC";
    WScript.Echo("Converting solution to " + convertKeyFriendly + 
            ". This may take several minutes.");
    
    var res = -1;
    try
    {
        if (logStream)
            logStream.WriteLine("Converting " + cmdICConvert);
            
        res = WshShell.Run(cmdICConvert, 7, true);
        if (0 != res)
            WScript.Echo("Conversion finished with code " + res);
            
        if (logStream)
            logStream.WriteLine("Conversion to " + convertKeyFriendly + 
                " finished with code " + res);
    }
    catch(e)
    {
        WScript.Echo("Conversion failed");
        if (logStream)
            logStream.WriteLine("Conversion failed");
            
        return -1;
    }
    
    return res;
}

//////////////////////////////////////////////////////////////////////
// compile environment
var activeCompiler = null;
var activeLinker = null;
var activeLibrarian = null;

var logFileName = "config.log";

// Setup compiler and linker names and options
function setCompileEnvironment(solutionName, configurationName
                                , projectName, logFile)
{
    // obtain the solution. Check that there is no specail solution
    // for configure script
    var solution = configurations.get(solutionName);
    if (null != configurations.get(solutionName + "_config"))
        solution = configurations.get(solutionName + "_config");
        
    var solutionconfig = solution.configurations.get(configurationName);
    var projectConfig = 
        solutionconfig.projectConfigurations.get(projectName);
    var project = solution.projects.get(projectConfig.projectName);
    var platform = project.platforms.get(projectConfig.platform);
    var configuration = 
        platform.configurations.get(projectConfig.configuration);

    activeCompiler = configuration.tools.get(compilerToolName);
    activeLinker = configuration.tools.get(linkerToolName);
    activeLibrarian = configuration.tools.get(librarianToolName);

    logFileName = logFile;
}

// performs compilation using provided compiler
function compile(compiler)
{
    var command = compiler.getCommandLine();
    var message = "Compiling with command \"" + command + "\"";
    logLine(message);
    return WshShell.Run("cmd /c \"" + command +"\" >> " + 
        logFileName + " 2>&1", runWindowMode, true);
}

// performs compilation using active compiler
function compileFiles(srcs, defines)
{
    var compiler = activeCompiler.clone();
    var srcsArr = srcs.split(" ");
    for (i in srcsArr)
    {
        compiler.inputFiles.add(srcsArr[i]);
    }
    if (typeof(defines) == "string")
    {
        defines = Array(defines);
    }
    if (defines instanceof Array)
    {
        for (i in defines)
        {
            compiler.defines.add(defines[i]);
        }
    }
    return compile(compiler);
}

// links using provided linker
function link(linker)
{
     var command = linker.getCommandLine();
     var message = "Linking with command \"" + command + "\"";
     logLine(message);
     return WshShell.Run("cmd /c \"" + command +"\" >> " 
        + logFileName + " 2>&1", runWindowMode, true);
}

// links using active linker
function linkFiles(srcs, outName, linker)
{
     if (!linker)
     {
        linker = activeLinker.clone();
     }
     linker.outputFile = outName;
     var srcsArr = srcs.split(" ");
     for (i in srcsArr)
     {
        linker.inputFiles.add(srcsArr[i]);
     }
     return link(linker);
}

// preprocesses using provided compiler
function preprocess(compiler, preprocFile)
{
     var command = compiler.getPreprocessCommandLine();
     var message = "Preprocessing with command \"" + command + "\"";
     logLine(message);
     return WshShell.Run("cmd /c \"" + command +"\" > " 
        + preprocFile + " 2>> " + logFileName, runWindowMode, true);
}

// preprocesses using active compiler
function preprocessFile(src, preprocFile)
{
     var compiler = activeCompiler.clone();
     compiler.inputFiles.add(src);
     return preprocess(compiler, preprocFile);
}

// links a library using provided librarian
function buildLibrary(librarian)
{
     var command = librarian.getCommandLine();
     var message = "Making library with command \"" + command + "\"";
     logLine(message);
     return WshShell.Run("cmd /c \"" + command +"\" >> " 
        + logFileName + " 2>&1", runWindowMode, true);
}

// links a library using active librarian
function makeLibrary(srcFiles, outDir, outFile, isShared)
{
    var ret = compileFiles(srcFiles);
    if (ret != 0)
    {
        return ret;
    }
    var objNames = srcFiles.replace(/(?:[\S]+[/\\\\])?([^/\\\\]+\.)cpp/gi, 
        outDir + "/$1obj");
        
    if (isShared)
    {
        // call linker to build a dll
        var linker = activeLinker.clone();
        linker.isDLL = true;
        return linkFiles(objNames, outFile, linker);
    }
    // call librarian to build lib
    var librarian = activeLibrarian.clone();
    librarian.outputFile = outFile;
    var objsArray = objNames.split(" ");
    for (i in objsArray)
    {
        librarian.inputFiles.add(objsArray);
    }
    return buildLibrary(librarian);
}

// logs text line into a log file
function logLine(line)
{
    var stream = fso.OpenTextFile(logFileName, 8, true, 0);
    stream.WriteLine(line);
    stream.Close();
}
