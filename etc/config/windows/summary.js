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

// BuildLog.htm file constants

var cmdLineTag = "Command Lines";
var outputTag = "Output Window";
var summaryTag = "Results";

var errorsTag = "error(s)";
var warningsTag = "warning(s)";

var compilingTag = "Compiling...";
var linkingTag = "Linking...";

var hdrLibrary = "Library Summary";
var hdrTestDriver = "Test Driver Summary";
var hdrExamples = "Example Summary";
var hdrTests = "Test Summary";

////////////////////////////////////////////////////////////////////

// read BuildLog.htm  
function readBuildLog(exeDir, itemInfo, useUnicode)
{
    if (! fso.FolderExists(exeDir))
        return;
        
    var blogDir = exeDir + "\\" + itemInfo.name;
    
    if (! fso.FolderExists(blogDir))
        return;
        
    var blogFilePath = blogDir + "\\" + buildlogFile;
    if (! fso.FileExists(blogFilePath))
        return;

    var uniMode = (true == useUnicode) ? -1 : 0;
    var blogFile;

    try
    {
        blogFile = fso.OpenTextFile(blogFilePath, 1, false, uniMode);
    }
    catch (e)
    {
        WScript.Echo("Cannot open file: " + blogFilePath);
        return;
    }

    var blogData = blogFile.AtEndOfStream ? "" : blogFile.ReadAll();
    
    var posTmp = getCommandLinesInfo(itemInfo, blogData, 0);
    posTmp = getCompilationInfo(itemInfo, blogData, posTmp);
    posTmp = getBuildSummaryInfo(itemInfo, blogData, posTmp);
}

// BuildLog.htm parsing methods 
function getCommandLinesInfo(itemInfo, blogData, posStart)
{
    //lookup for the command lines block
    var posCmdLines = blogData.indexOf(cmdLineTag, posStart);
    if (-1 == posCmdLines)
        return posStart;
            
    // extract table in the command lines block 
    itemInfo.buildCmdLog = extractTableData(blogData, posCmdLines);
    return posStart + itemInfo.buildCmdLog.length;
}


function getCompilationInfo(itemInfo, blogData, posStart)
{
    //lookup for the output block
    var posCmplInfo = blogData.indexOf(outputTag, posStart);
    if (-1 == posCmplInfo)
        return posStart;
        
    // extract table in the output block    
    itemInfo.buildOutLog = extractTableData(blogData, posCmplInfo);
    return posStart + itemInfo.buildOutLog.length;
}


function getBuildSummaryInfo(itemInfo, blogData, posStart)
{
    //lookup for the results block
    var posResInfo = blogData.indexOf(summaryTag, posStart);
    if (-1 == posResInfo)
        return posStart;
        
    // extract table in the results block   
    var summaryData = extractTableData(blogData, posResInfo);
    
    // skip first line in the summary as not needed at all
    var posPrjName = summaryData.indexOf(buildlogFile);
    var posPrjTmp = posPrjName;
    posPrjName = summaryData.indexOf(String.fromCharCode(10), posPrjTmp);
    if (-1 == posPrjName)
    {
        posPrjName = summaryData.indexOf("\r\n", posPrjTmp);
        if (-1 == posPrjName)
            return posStart + summaryData.length;
    }
        
    var prjSummary = summaryData.substr(posPrjName);
    
    // we not needed in tags, so remove them
    var tagIdx = prjSummary.indexOf("<");
    itemInfo.buildOutLog += "\r\n" + prjSummary.substr(0, tagIdx);
        
    // parse project summary and get number of errors and warnings
    var rgSum = prjSummary.split(" ");
    
    itemInfo.errorsCnt = rgSum[2];
    itemInfo.warningsCnt = rgSum[4];
    
    // check what kind of errors we have
    if (itemInfo.errorsCnt != "0")
    {
        var posLinking = itemInfo.buildOutLog.indexOf(linkingTag);
        if (posLinking != -1)
            itemInfo.linkerErrors = true;
    }
    
    return posStart + summaryData.length;
}

function extractTextBlockData(blogData, posTextBlock)
{
    var nextTableBlock = blogData.indexOf("<table", posTextBlock);
    
    var textData = "";
    if (nextTableBlock > 0)
    {
        textData = 
            blogData.substr(posTextBlock, nextTableBlock - posTextBlock);
    }
    else
    {
        textData = blogData.substr(posTextBlock);
    }
    
    var posPreTag = textData.indexOf("<pre", 0);
    if (-1 == posPreTag)
        textData = "<pre>" + textData + "</pre>";
        
    return textData;
}

function extractTableData(blogData, posStart)
{
    var posMainTable = blogData.indexOf("<table", posStart);
    if (-1 == posMainTable)
        return "";
        
    // check that we have data in table form
    var posFirstCloseTable = blogData.indexOf("</table>", posStart);
    if (-1 != posFirstCloseTable && posMainTable - posFirstCloseTable > 20)
        return extractTextBlockData(blogData, posFirstCloseTable + 8);
        
    // lookup for the main table close tag 
    var posCloseTag = posMainTable;
    var posOpenTag = posMainTable;
    while (true)
    {
        posCloseTag = blogData.indexOf("</table>", posCloseTag + 5);
        if (-1 == posCloseTag) // no close tag?
            break;
            
        posOpenTag = blogData.indexOf("<table", posOpenTag + 5);
        if (-1 == posOpenTag)
            break;
        
        if (posCloseTag < posOpenTag)
            break;
    }
    
    var tableData = "";
    var tableDataLen = posCloseTag + 8 - posMainTable;
    if (tableDataLen > 0)
        tableData = blogData.substr(posMainTable, tableDataLen);
    else
        tableData = blogData.substr(posMainTable);
        
    // visual studio doesn't provide last table row close tags
    // add them here
    var indexClose = tableData.lastIndexOf("</table>");
    tableData = tableData.substr(0, indexClose) + "</td></tr></table>";
    
    return tableData;
}

////////////////////////////////////////////////////////////////////
// get differences method

function getDifferencesInfo(itemInfo)
{
    var diffs = getWinDiffDifferences(itemInfo.runOutput, 
        itemInfo.runReqOutput, "ILFRGX");
        
    // replace first line
    var endLine = String.fromCharCode(13) + String.fromCharCode(10);
    var pos = diffs.indexOf(endLine);
    if (-1 == pos)
    {
        itemInfo.runDiff = diffs;
        return;
    }
    
    itemInfo.runDiff = "Differences between real output and required: ";
    itemInfo.runDiff += diffs.substr(pos);
    
    //WScript.Echo(itemInfo.runDiff);
}

///////////////////////////////////////////////////////////////////////
// save methods

function saveBuildSummary(itemInfo, fileSummary)
{
    var failed = (itemInfo.exitCode != 0 || itemInfo.errorsCnt != "0") 
        ? true : false;
    
    if (failed == true)
        fileSummary.WriteLine("<tr bgcolor=\"#FF9999\">");
    else
        fileSummary.WriteLine("<tr bgcolor=\"#FFFF99\">");
    
    var someCol = "<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\" align=\"left\">";
    var lastCol = 
        "<td style=\"padding:2px;border-bottom:#000000 solid 1px;\" \
        align=\"right\">";
        
    fileSummary.Write(someCol);
    fileSummary.Write("<a href=\"#" + itemInfo.name + "\">");
    fileSummary.WriteLine(itemInfo.name + "</a></td>");
    
    if (itemInfo.errorsCnt != "0")
    {
        fileSummary.WriteLine(someCol + "Build Failed" + "</td>");
    }
    else
    {
        if (failed == true)
        {
            fileSummary.WriteLine(someCol + "Exited with code " + 
                itemInfo.exitCode + "</td>");
        }
        else
        {
            fileSummary.WriteLine(someCol + "Succeeded" + "</td>");
        }   
    }
    
    if (failed == false)
    {
        if (itemInfo.runDiff != "")
            fileSummary.WriteLine(someCol + "Difference" + "</td>");
        else 
            fileSummary.WriteLine(someCol + "Equal" + "</td>");
    }
    else
    {
        fileSummary.WriteLine(someCol + "&nbsp;" + "</td>");
    }

    fileSummary.WriteLine(lastCol + itemInfo.warningsCnt + "</td>");
        
    fileSummary.WriteLine("</tr>");
}

function saveBuildInfo(itemInfo, infoDir, infoExt)
{
    var outfileName = infoDir + "\\" + itemInfo.name + "." + infoExt;
    var outFile = fso.CreateTextFile(outfileName);
    
    saveBuildInfoTable(outFile, itemInfo, hdrExamples, false);

    outFile.Close();
}

function saveBuildInfoTable(outFile, itemInfo, linkSummary, bLib)
{
    outFile.WriteLine("<table border=\"0\" width=\"100%\" \
        style=\"border:#000000 solid 1px;\"  cellspacing=\"0\">");
        
    outFile.Write("<tr><td bgcolor=\"#CCCCCC\" \
        style=\"border-bottom:#000000 solid 1px;\"><strong>");
    outFile.Write("<a name=\"" + itemInfo.name + "\">" + 
        itemInfo.name + "</a>");
    outFile.WriteLine("</strong>&nbsp;<a href=\"#" + 
        makeLinkFromString(linkSummary) + "\"><i>(" + 
        linkSummary.toLowerCase() + 
        ")</i></a>&nbsp;<a href=\"#top\"><i>(top)</i></a></td></tr>");

    saveBuildInfoBlock(outFile, "Build Command Lines", 
        itemInfo.buildCmdLog, false);
    saveBuildInfoBlock(outFile, "Build Output", 
        itemInfo.buildOutLog, false);
    
    if (false == bLib)
    {
        if (itemInfo.runOutput == "" && itemInfo.exitCode != 0)
            itemInfo.runOutput = "Exited with code " + itemInfo.exitCode;
            
        saveBuildInfoBlock(outFile, "Executable Output",
            encodeHTML(itemInfo.runOutput), true);
    }
    
    if (itemInfo.runDiff != "")
    {
        saveBuildInfoBlock(outFile, "Differences", 
            encodeHTML(itemInfo.runDiff), true);
    }
            
    outFile.WriteLine("</table>");
}


function saveBuildInfoBlock(outFile, blockName, blockData, needPre)
{
    // header
    outFile.Write("<tr><td style=\"padding:4px;\"><strong>");
    outFile.Write(blockName);
    outFile.WriteLine("</strong></td></tr>");

    // data
    outFile.WriteLine("<tr><td widht=\"100%\" \
        style=\"padding-left:4px;padding-right:4px;padding-bottom:4px;\">");
        
    outFile.WriteLine("<table  width=\"100%\" border=\"0\" \
        bgcolor=\"#EEEEEE\" \
        style=\"left-margin:5px;right-margin:\
        5px;border:#000000 solid 1px;\"> ");
        
    outFile.WriteLine("<tr> <td>");
    
    if (needPre == true)
        outFile.WriteLine("<pre>");
        
    outFile.WriteLine(blockData);
    
    if (needPre == true)
        outFile.WriteLine("</pre>");

    outFile.WriteLine("</td></tr>");
    outFile.WriteLine("</table>");
    outFile.WriteLine("</td></tr>");
}

//////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

function makeLinkFromString(linkString)
{
    var srcS = " ";
    var destS = "-";
    var reRep = new RegExp(srcS, "mg");
    var res = linkString.replace(reRep, destS);
    res = res.toLowerCase();
    return res;
}

function getSummaryLogPath(buildDir, buildSummaryPrefix, buildType)
{
    var outDir = buildDir;
    return outDir + "\\" + buildSummaryPrefix + buildType + ".html";
}

function makeSummaryLog(buildDir, buildSummaryPrefix, buildType)
{
    var outDir = buildDir;
    if (! fso.FolderExists(outDir))
        fso.CreateFolder(outDir);
        
    var oFolder = fso.GetFolder(outDir);
    var sumFileName = buildSummaryPrefix + buildType + ".html";
    var fSum = oFolder.CreateTextFile(sumFileName);
    
    fSum.WriteLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fSum.WriteLine("<body bgcolor=\"#FFFFFF\"> \
        <font face=\"arial\" color=\"#000000\">");
        
    fSum.WriteLine("<center><H2>" + buildType + "</H2></center>");
    
    fSum.WriteLine("<table border=\"0\" width=\"100%\">");
    
    fSum.WriteLine("<tr><td style=\"padding-top:5px;padding-bottom:5px; \
        border-top:#000000 solid 1px;border-bottom:#000000 solid 1px;\" \
        align=\"center\">");
        
    fSum.WriteLine("<a href=\"#" + makeLinkFromString(hdrLibrary) + 
        "\">" + hdrLibrary + "</a>&nbsp;:&nbsp;");
    fSum.WriteLine("<a href=\"#" + makeLinkFromString(hdrTestDriver) + 
        "\">" + hdrTestDriver + "</a>&nbsp;:&nbsp;");
    fSum.WriteLine("<a href=\"#" + makeLinkFromString(hdrExamples) + 
        "\">" + hdrExamples + "</a>&nbsp;:&nbsp;");
    fSum.WriteLine("<a href=\"#" + makeLinkFromString(hdrTests) + 
        "\">" + hdrTests + "</a>");
        
    fSum.WriteLine("</td></tr>");
    
    fSum.WriteLine("</table>");
    fSum.WriteLine("<br/>");
    
    return fSum;
}


function saveSummaryHeaderLib(fSum, libInfo, hdrLibrary)
{
    fSum.WriteLine("<strong><a name=\"" + makeLinkFromString(hdrLibrary) + 
        "\">" + hdrLibrary + "</a></strong>");
        
    fSum.WriteLine("<table border=\"0\" width=\"100%\" \
        style=\"border:#000000 solid 1px;\"  cellspacing=\"0\">");
        
    // save header row
    fSum.WriteLine("<tr bgcolor=\"#CCCCCC\">");
    fSum.WriteLine(
        "<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\"><strong>Library</strong></td>");
    fSum.WriteLine(
        "<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\"><strong>State</strong></td>");
    fSum.WriteLine(
        "<td style=\"padding:2px;border-bottom:#000000 solid 1px;\">\
        <strong>Warnings</strong></td>");
    fSum.WriteLine("</tr>");
    
    // save library header
    if (libInfo.errorsCnt == "0")
        fSum.WriteLine("<tr bgcolor=\"#FFFFFF\" \
            style=\"border-bottom:#000000 solid 1px;\">");
    else
        fSum.WriteLine("<tr bgcolor=\"#FF9999\" \
            style=\"border-bottom:#000000 solid 1px;\">");
        
    fSum.WriteLine(
        "<td style=\"padding:2px;border-right:#000000 solid 1px;\">"
        + "<a href=\"#" + libInfo.name + "\">" + libInfo.name + "</a></td>");
        
    fSum.Write("<td style=\"padding:2px;border-right:#000000 solid 1px;\">");
    if (libInfo.errorsCnt == "0")
        fSum.Write("OK");
    else
        fSum.Write("Failed");
    fSum.WriteLine("</td>");
    
    fSum.WriteLine("<td style=\"padding:2px;\">" + 
        libInfo.warningsCnt + "</td>");
    
    fSum.WriteLine("</tr>");
    
    fSum.WriteLine("</table>");
    fSum.WriteLine("<br/>");
}

function saveSummaryHeaderTestDriver(fSum, rwtestInfo, hdrDriver)
{
    fSum.WriteLine("<strong><a name=\"" + makeLinkFromString(hdrDriver) + 
        "\">" + hdrDriver + "</a></strong>");
        
    fSum.WriteLine("<table border=\"0\" width=\"100%\" \
        style=\"border:#000000 solid 1px;\"  cellspacing=\"0\">");
        
    // save header row
    fSum.WriteLine("<tr bgcolor=\"#CCCCCC\">");
    fSum.WriteLine("<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\">\
        <strong>Test Driver</strong></td>");
    fSum.WriteLine(
        "<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\"><strong>State</strong></td>");
    fSum.WriteLine(
        "<td style=\"padding:2px;border-bottom:#000000 solid 1px;\">\
        <strong>Warnings</strong></td>");
    fSum.WriteLine("</tr>");
    
    // save test driver header
    if (rwtestInfo.errorsCnt == "0")
        fSum.WriteLine("<tr bgcolor=\"#FFFFFF\" \
            style=\"border-bottom:#000000 solid 1px;\">");
    else
        fSum.WriteLine("<tr bgcolor=\"#FF9999\" \
            style=\"border-bottom:#000000 solid 1px;\">");
        
    fSum.WriteLine(
        "<td style=\"padding:2px;border-right:#000000 solid 1px;\">" + 
        "<a href=\"#" + rwtestInfo.name + "\">" + 
        rwtestInfo.name + "</a></td>");
        
    fSum.Write(
        "<td style=\"padding:2px;border-right:#000000 solid 1px;\">");
    if (rwtestInfo.errorsCnt == "0")
        fSum.Write("OK");
    else
        fSum.Write("Failed");
    fSum.WriteLine("</td>");
    
    fSum.WriteLine("<td style=\"padding:2px;\">" + 
        rwtestInfo.warningsCnt + "</td>");
    
    fSum.WriteLine("</tr>");
    
    fSum.WriteLine("</table>");
    fSum.WriteLine("<br/>");
}


function saveSummaryHeaderMulti(fSum, exsDir, buildType, hdrExamples)
{
    fSum.WriteLine("<strong><a name=\"" + makeLinkFromString(hdrExamples) + 
        "\">" + hdrExamples + "</a></strong>");
        
    fSum.WriteLine("<table border=\"0\" width=\"100%\" \
        style=\"border:#000000 solid 1px;\"  cellspacing=\"0\">");
        
    // save header row
    fSum.WriteLine("<tr bgcolor=\"#CCCCCC\">");
    fSum.WriteLine("<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\"><strong>Example</strong></td>");
    fSum.WriteLine("<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\"><strong>State</strong></td>");
    fSum.WriteLine("<td style=\"padding:2px;border-right:#000000 solid 1px;\
        border-bottom:#000000 solid 1px;\">\
        <strong>Comparision</strong></td>");
    fSum.WriteLine("<td style=\"padding:2px;\
        border-bottom:#000000 solid 1px;\"><strong>Warnings</strong></td>");
    fSum.WriteLine("</tr>");
    
    // load information from local summary file 
    // and save it to general summary
    var lsumFileName = exsDir + "\\" + summaryFileName;
    if (fso.FileExists(lsumFileName))
    {
        var fileLSum = fso.OpenTextFile(lsumFileName);
        if (!fileLSum.AtEndOfStream)
        {
            var lsumData = fileLSum.ReadAll();
            fSum.Write(lsumData);
        }
        fileLSum.Close();
    }
    
    fSum.WriteLine("</table>");
    fSum.WriteLine("<br/>");
}

function saveBuildSummarySingle(fSum, libInfo, hdrLibrary)
{
    saveBuildInfoTable(fSum, libInfo, hdrLibrary, true);
    fSum.WriteLine("<br/>");
}


function saveBuildSummaryMulti(fSum, htmDir)
{
    if (!fso.FolderExists(htmDir))
        return;
    var htmFolder = fso.GetFolder(htmDir);
        
    var enumHtmSubFolders = new Enumerator(htmFolder.SubFolders);
    for (; !enumHtmSubFolders.atEnd(); enumHtmSubFolders.moveNext())
    {
        var htmFName = enumHtmSubFolders.item().Name;
        saveBuildSummariesFromFolder(fSum,
            enumHtmSubFolders.item().Path, htmFolderName);
        saveBuildSummaryMulti(fSum, htmDir + "\\" + htmFName);
    }
}

function saveBuildSummariesFromFolder(fSum, testFolder, htmFName)
{
    var htmFldName = testFolder + "\\" + htmFName;
    if (!fso.FolderExists(htmFldName))
        return;
        
    var htmFld = fso.GetFolder(htmFldName);
        
    var rx = new RegExp("^.+\\.(?:htm)$", "i");
    var enumHtmFiles = new Enumerator(htmFld.Files);
    for (; !enumHtmFiles.atEnd(); enumHtmFiles.moveNext())
    {
        var htmFileName = enumHtmFiles.item().Name;
        if (! rx.test(htmFileName))
            continue;
            
        var htmFile = fso.OpenTextFile(htmFldName + "\\" + htmFileName);
        var htmData = htmFile.ReadAll();
            
        fSum.Write(htmData);

        fSum.WriteLine("<br/>");
    }
}
        
        
function closeSummaryLog(fSum)
{
    fSum.WriteLine("</body>");
    fSum.Close();
}

////////////////////////////////////////////////////////////////////////////
function checkForFailures(testDir, bType, logHtm, sumHtm, htmTempDir, 
            seeHtm, useUnicode)
{
    if (!fso.FolderExists(testDir))
        return;
    var testFolder = fso.GetFolder(testDir);
        
    var seeHtmHere = seeHtm;
    if (false == seeHtmHere && testFolder.Name == bType)
        seeHtmHere = true;
        
    var enumHtmSubFolders = new Enumerator(testFolder.SubFolders);
    for (; !enumHtmSubFolders.atEnd(); enumHtmSubFolders.moveNext())
    {
        var htmFName = enumHtmSubFolders.item().Name;
        checkForFailures(testDir + "\\" + htmFName, bType,
            logHtm, sumHtm, htmTempDir, seeHtmHere, useUnicode);
    }
 
    if (false == seeHtmHere)
        return;
         
    var rx = new RegExp("^.+\\.(?:htm)$", "i");
    var enumHtmFiles = new Enumerator(testFolder.Files);
    for (; !enumHtmFiles.atEnd(); enumHtmFiles.moveNext())
    {
        var htmFileName = enumHtmFiles.item().Name;
        if (! rx.test(htmFileName))
            continue;
            
        if (htmFileName != logHtm)
            continue;
            
        var testInfo = new ItemBuildInfo(testFolder.Name);
        
        var uniMode = (true == useUnicode) ? -1 : 0;
        var blogFile = 
            fso.OpenTextFile(testFolder.Path + "\\" + htmFileName, 
                             1, false, uniMode);
        var blogData = blogFile.AtEndOfStream ? "" : blogFile.ReadAll();
    
        var posTmp = getCommandLinesInfo(testInfo, blogData, 0);
        posTmp = getCompilationInfo(testInfo, blogData, posTmp);
        posTmp = getBuildSummaryInfo(testInfo, blogData, posTmp);
        
        if (testInfo.errorsCnt != "0")
            saveBuildFailure(testFolder.Path, testInfo, sumHtm, htmTempDir);
    }
}

function saveBuildFailure(testDir, testInfo, sumHtm, htmTempDir)
{
    var htmTempPath = getParentFolder(testDir) + "\\" + htmTempDir;
    if (! fso.FolderExists(htmTempPath))
        fso.CreateFolder(htmTempPath);
    
    saveBuildInfo(testInfo, htmTempPath, "htm");
    
    var sumTempFile;
    if (fso.FileExists(sumHtm))
    {
        sumTempFile = fso.OpenTextFile(sumHtm, 8);
    }
    else
    {
        WScript.Echo("Path " + sumHtm + " not found");
        return;
    }
        
    saveBuildSummary(testInfo, sumTempFile);
    sumTempFile.Close();
}
