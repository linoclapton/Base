if exists("b:current_syntax")
    finish
endif

let b:current_syntax = "shader"

syn keyword sKeyword for while
syn keyword sCondition if else switch
syn keyword sKeyword class return
syn keyword	sRepeat	while for do

highlight link sKeyword Keyword
highlight link sRepeat Keyword
