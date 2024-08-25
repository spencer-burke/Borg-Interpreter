# Borg-Interpreter
This is a simple vm which which interprets the "borg" pseudo programming language.
It has comments, variables, and basic math.
The vm also supports the idea of scope.
"START" and "FINISH" blocks will determine scope.
It will use the "highest" scope, or prioritize local variables.
## Sample BORG Program
```
COM VALID BORG COMMENT
COM ANOTHER COMMENT

COM THIS COM IS A NEWLINE AWAY

START
    VAR ABC = 25
    VAR DEF = 13
    VAR GHI = 17
    VAR JKL = 10
    START
        VAR ABC = 12
        PRINT GHI
        PRINT 2 * GHI
        PRINT ABC / JKL
        PRINT MNO
        VAR MNO = 15
        VAR QRS = 2
        VAR TUV = 15.5
        MNO++
        PRINT MNO
        MNO--
        PRINT MNO
        DEF = 26
        PRINT DEF
    FINISH
FINISH
```
## Output
```
GHI IS 17
2 * GHI IS 34
ABC / JKL IS 1.2
MNO IS UNDEFINED
MNO IS 16
MNO IS 15
DEF IS 26
```