#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.
;^ ctrl ! alt # win
; :*:.::@a
;:*:dd::
;send,{Home}
;send,{LShift Down}{End}{LShift Up}{BS}
;return
::ftw::for the whale
::btw::
 MsgBox You typed "btw"
Return
^k::
    sendinput, inside the crtl{+}j hotkey
return
Numpad0::
run, Notepad
return
#IfWinActive ahk_class AcrobatSDIWindow
!j::
send,{Down}
return
!k::
send,{Up}
return
#IfWinActive
#IfWinActive ahk_exe devenv.exe 
^h::
send, {BS}
return
#IfWinActive

#IfWinActive ahk_class Vim
^m::
send,{Enter} 
return
#IfWinActive
;#IfWinActive ahk_class Notepad
!j::
   Send, {down}
Return
!k::
   Send, {up}
Return
!l::
   Send, {right}
Return
!h::
   Send, {left}
Return
!i::
   Send, {HOME}
return
!a::
   Send, {END}
return
!o::
   Send, {Home}{Enter}{Up}
return
^h::
send, {BS}
return
^m::
keywait, Ctrl
;blockInput, On
send, {Enter}
;blockInput, Off
return
;#IfWinActive ahk_exe devenv.exe
;keywait, Ctrl
;sendInput, {Ctrl down}{m}{Ctrl up}{Ctrl down}{o}{Ctrl up}
;blockInput, On
;blockInput, Off
;return
;#IfWinActive

#s::
run, http://cn.bing.com
return

#g::
run, gvim
return

#c::
run, cmd 
return

#n::
run, notepad
return



#IfWinActive ahk_exe SGTool.exe
^h::
send, {BS}
return
#IfWinActive

!1::
MouseClick, left,1440, 1080,  1,0
send, #{Tab} 
sleep, 250
send, {Tab} {Enter}
MouseClick, left, 980, 540,  1,0
return

!2::
MouseClick, left,1440, 1080,  1,0
send, #{Tab} 
sleep, 250
send, {Tab} {Right} {Enter}
MouseClick, left, 980, 540,  1,0
return

!3::
MouseClick, left,1440, 1080,  1,0
send, #{Tab} 
sleep, 250
send, {Tab} {Right} {Right} {Enter}
MouseClick, left, 980, 540,  1,0
return

!Space::
MouseClick, left,177,103 
return



