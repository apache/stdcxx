//
// $Id$
//
// filterdef.js - FilterDef class definition
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

var sourceFilterName = "Source Files";
var sourceFilterUuid = "{4FC737F1-C7A5-4376-A066-2A32D752A2FF}";
var sourceFilterExts = ".cpp;.cxx;.s;.asm";

var headerFilterName = "Header Files";
var headerFilterUuid = "{93995380-89BD-4b04-88EB-625FBE52EBFB}";
var headerFilterExts = ".h;.hpp;.hxx;.c;.cc";

//------------------------------------------------
// Macro class
//------------------------------------------------

// Macro .ctor
function Macro(name, value)
{
    this.name = name;
    this.value = value;
}

// Replace all macros in str
// str - string to modify
// arrMacro - array of Macro objects
function ReplaceMacros(str, arrMacro)
{
    for (var i = 0; i < arrMacro.length; ++i)
    {
        var macro = arrMacro[i];
        str = str.replace(new RegExp("(" + macro.name + ")", "g"), macro.value)
    }
    
    return str;
}

// common macros
var cmnMacros = new Array();

// init custom build rule for .asm files
function InitAsmTool(VCFile)
{
    var cfgs = VCFile.FileConfigurations;
    for (var i = 1; i <= cfgs.Count; ++i)
    {
        var cfg = cfgs.Item(i);
        if ((typeof(cfg.Tool.ToolKind) != "undefined" &&
            cfg.Tool.ToolKind != "VCCustomBuildTool") ||
            cfg.Tool.ToolName != "Custom Build Tool")
        {
            cfg.Tool = cfg.ProjectConfiguration.FileTools.Item("VCCustomBuildTool");
        }

        var tool = cfg.Tool;
        tool.Description = "Compiling .asm file...";
        tool.Outputs = "$(IntDir)\\$(InputName).obj";
        tool.CommandLine = AS + " /c /nologo /D" + PLATFORM + " /Fo" + tool.Outputs +
                           " /W3 /Zi /Ta" + VCFile.RelativePath;
    }
}

//------------------------------------------------
// FilterDef class
//------------------------------------------------

// FilterDef .ctor
function FilterDef(name, id, filter, type, exclude)
{
    this.Name = name;
    this.Id = id;
    this.Filter = filter;
    this.Type = type;
    this.Exclude = exclude;
    this.Folder = null;
    this.Files = new Array();
    this.exclFolders = null;
    this.exclFiles = null;
    this.FilterDefs = new Array();
}

FilterDef.prototype.addFilter = filterAddFilter;
FilterDef.prototype.addFiles = filterAddFiles;
FilterDef.prototype.addFilesByMask = filterAddFilesByMask;
FilterDef.prototype.createVCFilter = filterCreateVCFilter;

// add subfilter to object
function filterAddFilter(filter)
{
    this.FilterDefs.push(filter);
    return this;
}

// add files to object
// folder - parent folder
// files - filename of array if filenames
function filterAddFiles(folder, files)
{
    this.Folder = folder;
    if (files instanceof Array)
        this.Files = this.Files.concat(files);
    else
        this.Files.push(files);
    return this;
}

// add to object files from filder and all subfolders
// excluding exclFolders and exclFiles
// folder - start folder
// exclFolder - regular expression which defines the folders to exclude
// exclFiles - regular expression which defines the files to exclude
function filterAddFilesByMask(folder, exclFolders, exclFiles)
{
    this.Folder = folder;
    this.exclFolders = exclFolders;
    this.exclFiles = exclFiles;
    return this;
}

// add file to VCFilter object
// filter - VCFilter object
// filename - filename to add
// filetype - type of file (one of eFileTypexxx)
// exclude - if true then file will be excluded from build
function AddFilterFile(filter, filename, filetype, exclude)
{
    var VCFile = filter.AddFile(filename);
    if (null != filetype && typeof(VCFile.FileType) != "undefined")
        VCFile.FileType = filetype;

    var customFileDef = null;

    if (exclude)
    {
        var cfgs = VCFile.FileConfigurations;
        for (var i = 1; i <= cfgs.Count; ++i)
        {
            var cfg = cfgs.Item(i);
            if (typeof(cfg.Tool.ToolKind) != "undefined")
            {
                if (cfg.Tool.ToolKind != "VCCLCompilerTool")
                    exclude = false;
            }
            else if (cfg.Tool.ToolName != "C/C++ Compiler Tool")
                exclude = false;

            cfg.ExcludedFromBuild = exclude;
        }
    }
    else if (".asm" == VCFile.Extension)
        InitAsmTool(VCFile);
}

// create VCFilter object from the FilterDef definition
// and add to parent
function filterCreateVCFilter(parent)
{
    var VCFilter;
    if (null == this.Name)
        VCFilter = parent;
    else
    {
        VCFilter = parent.AddFilter(this.Name);
        if (null != this.Id)
            VCFilter.UniqueIdentifier = this.Id;
        if (null != this.Filter)
            VCFilter.Filter = this.Filter;
    }

    if (null != this.Folder)
        this.Folder = ReplaceMacros(this.Folder, cmnMacros);
    
    if (0 < this.Files.length)
    {
        // add specified files
        for (var i = 0; i < this.Files.length; ++i)
        {
            var filename = this.Files[i];
            if (null != this.Folder && this.Folder.length > 0)
                filename = this.Folder + "\\" + filename;
            
            try
            {
                fso.GetFile(filename);
            }
            catch (e)
            {
                WScript.Echo("File " + filename + " does not exist");
                WScript.Quit(3);
            }
            
            AddFilterFile(VCFilter, filename, this.Type, this.Exclude);
        }
    }
    else
    {
        // add files from folder

        // create regexp from extensions
        var extArray = this.Filter.replace(/\./g, "\\.").split(";");
        var rxText = "^";
        if (extArray.length != 0)
        {
            rxText += "(?:" + extArray[0];
            for (i = 1; i < extArray.length; ++i)
                rxText += "|" + extArray[i];
            rxText += ")";
        }
        rxText += "$";
        
        var rxExts = new RegExp(rxText, "i");
        
        var folder;
        
        try
        {
            folder = fso.GetFolder(this.Folder);
        }
        catch (e)
        {
            WScript.Echo("Folder " + this.Folder + " does not exist");
            WScript.Quit(3);
        }
        
        // add subfolders as own filters
        var enumSubFolders = new Enumerator(folder.SubFolders);
        for (; !enumSubFolders.atEnd(); enumSubFolders.moveNext())
        {
            var subFolder = enumSubFolders.item();
            if (null == this.exclFolders || !this.exclFolders.test(subFolder.Name))
            {
                var filterDef = new FilterDef(subFolder.Name, this.Id,
                    this.Filter, this.Type, this.Exclude);
                filterDef.Folder = subFolder.Path;
                filterDef.exclFolders = this.exclFolders;
                filterDef.exclFiles = this.exclFiles;
                filterDef.createVCFilter(VCFilter);
            }
        }
        
        // add files
        var nfiles = 0;
        var enumFiles = new Enumerator(folder.Files);
        for (; !enumFiles.atEnd(); enumFiles.moveNext())
        {
            var file = enumFiles.item();
            var fileext = getExtension(file.Name);
            if (rxExts.test(fileext) && (null == this.exclFiles || !this.exclFiles.test(file.Name)))
            {
                ++nfiles;
                AddFilterFile(VCFilter, file.Path, this.Type, this.Exclude);
            }
        }
        
        // remove filter if it is empty
        if (0 == nfiles)
            parent.RemoveFilter(VCFilter);
    }
    for (var i = 0; i < this.FilterDefs.length; ++i)
        this.FilterDefs[i].createVCFilter(VCFilter);
}
