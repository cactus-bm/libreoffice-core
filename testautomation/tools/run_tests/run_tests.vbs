'*************************************************************************
'*
'*  OpenOffice.org - a multi-platform office productivity suite
'*
'*  $RCSfile: run_tests.vbs,v $
'*
'*  $Revision: 1.2 $
'*
'*  last change: $Author: andreschnabel $ $Date: 2008/09/05 16:56:20 $
'*
'*  The Contents of this file are made available subject to
'*  the terms of GNU Lesser General Public License Version 2.1.
'*
'*
'*    GNU Lesser General Public License Version 2.1
'*    =============================================
'*    Copyright 2005 by Sun Microsystems, Inc.
'*    901 San Antonio Road, Palo Alto, CA 94303, USA
'*
'*    This library is free software; you can redistribute it and/or
'*    modify it under the terms of the GNU Lesser General Public
'*    License version 2.1, as published by the Free Software Foundation.
'*
'*    This library is distributed in the hope that it will be useful,
'*    but WITHOUT ANY WARRANTY; without even the implied warranty of
'*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
'*    Lesser General Public License for more details.
'*
'*    You should have received a copy of the GNU Lesser General Public
'*    License along with this library; if not, write to the Free Software
'*    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
'*    MA  02111-1307  USA
'*
'########################################################################
'
' Owner : andreschnabel@openoffice.org
'
' short description :   run several testscripts on windows
'   
'   - list of scripts to run is read from stdin
'   - this script has been tested on WindowsXP and may run on Win2003
'       and Windows Vista
'   - the script does *not* Support Win9x / WinME or WindowsNT
'   - before you start the script set sLocation and sTestTool
'   - run this script with:
'       cscript.exe runtests.vbs <list_of_testscripts
'
'########################################################################


' set location of testscripts (the 'qatesttool' directory)
sLocation = "o:\qa\cvs\qa\qatesttool"

' set location of TestTool
' (full path including executable 'testtool.exe')
sTestTool = "O:\qa\qatesttool\win32\testtool.exe"


'------------------------------------------------------------------------
'--- the main script starts here ---
'------------------------------------------------------------------------
Dim oFSO	' AS FileSystemObject
Dim oStdIn	' As TextStream
Dim WshShell	' as WScript.Shell
Dim oExec	' as WshExec

' get Objects for Scripting
Set oFSO = CreateObject ("Scripting.FileSystemObject")
Set oStdIn =  WScript.StdIn
Set WshShell = CreateObject("WScript.Shell")


'--- platform specific settings
' Read Environment and do Windows Version specific stuff
' nothing done yet 


'--- if sLocation is not set manuall try to get the location form testtoolrc
If not oFSO.FolderExists(sLocation) Then
   '--- Read Location from testtool.ini
   WScript.Echo "Read Location from testtool.ini - not implemented yet"
End If

'--- set location of close-office file
' (see cvs)
sExitOfficeBas = sLocation & "global\tools\closeoffice.bas"
sResetOfficeBas = sLocation & "global\tools\resetoffice.bas"

'--- if sTestTool is not set manuall try to get the location form testtoolrc
If not oFSO.FileExists(sTestTool) Then
   '--- Read testtool path from testtool.ini
   WScript.Echo "Read testtool path from testtool.ini - not implemented yet"
End If


WScript.Echo "****************************************************"
WScript.Echo "************ STARTING ************"
WScript.Echo "****************************************************"

'echo "My name is: " $USER

' test if location exists
If oFSO.FolderExists(sLocation) Then
	WScript.Echo "using scripts from " & sLocation
Else
	WScript.Echo "test scripts not found at " & sLocation
	WScript.Echo "Please set sLocation in this script"
	WScript.Quit 1
End If

' test if testtool.bin exists
If oFSO.FileExists(sTestTool) Then
	WScript.Echo "using testtool from " & sTestTool
Else
	WScript.Echo "testtool not found at " & sTestTool
	WScript.Echo "Please set sTestTool in this script"
	WScript.Quit 1
End If

i = 0
'--- Main loop
While Not oStdIn.AtEndOfStream

    'WScript.echo "Running soffices' processes: "
    '# kill office, if exists
    '`$KILLOFFICEALL`        
    WshShell.Run "taskkill /F /IM soffice.exe /IM soffice.bin", 1, true 

    sTestCase = oStdIn.ReadLine

    WScript.echo " "
    WScript.Echo "****************************************************"
    WScript.Echo "running " & sTestCase
    sTest = sLocation & sTestCase

    If not oFSO.FileExists( sTest ) Then
        WScript.Echo " Could not run " & sTest 
        WScript.Echo " File not found"
    Else
        ' first run is the real test ...
        Set oExec = WshShell.Exec("""" & sTestTool & """ -run """ & sTest & """" )
        WScript.Sleep 1000

        If oExec.Status = 0 Then
            WScript.Echo " Successfully started"
        Else
            WScript.Echo " There might be something wrong with starting the Testtool!"
        End If

        '--- wait until Testtool has finished & closed
        While oExec.Status = 0
            WScript.Sleep 1000
            i = i+1
        Wend
    End If
    'run the office reset script!
    WScript.Echo " trying to reset office environment"
    WshShell.Run """" & sTestTool & """ -run """ & sResetOfficeBas & """", 1, true 
    'run the office exit script!
    WScript.Echo " trying to kill the previous office process"
    WshShell.Run """" & sTestTool & """ -run """ & sExitOfficeBas & """", 1, true 

    Set oExec = Nothing
Wend

WScript.Echo "Duration:" & i & "sec " 
