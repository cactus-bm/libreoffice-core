#! /bin/bash

LIKE=".*"
ONLY_ONE=0
DO_PPT=0
DO_PPT_HTML=0
DO_XLS=0
DO_XLS_HTML=0
DO_XLSX_XLS=0
DO_HTML_HIDE=0
DO_HTML=0
DO_DOC=0
DO_DOC_PDF=0
PARS=""
while [[ $# > 0 ]]
do
if [[ "X$1" == "Xclean" ]]
        then
                PARS=$PARS" "$1
                rm -r -f instdir
        fi
        if [[ "X$1" == "Xhtml" || "X$1" == "Xall" ]]
        then
                PARS=$PARS" "xlsx-html
                DO_HTML=1
        fi
        if [[ "X$1" == "Xhtml-hide" ]]
        then
                PARS=$PARS" "xlsx-html-hide
                DO_HTML_HIDE=1
        fi
        if [[ "X$1" == "Xdoc" || "X$1" == "Xall" ]]
        then
                PARS=$PARS" "doc-docx
                DO_DOC=1
        fi
        if [[ "X$1" == "Xdoc-pdf" || "X$1" == "Xall" ]]
        then
                PARS=$PARS" "doc-pdf
                DO_DOC_PDF=1
        fi
        if [[ "X$1" == "Xxls" || "X$1" == "Xall" ]]
        then
                PARS=$PARS" "xls-xlsx
                DO_XLS=1
        fi
        if [[ "X$1" == "Xxls-html" ]]
        then
                PARS=$PARS" "xls-html
                DO_XLS_HTML=1
        fi
        if [[ "X$1" == "Xxlsx-xls" ]]
        then
                PARS=$PARS" "xlsx-xls
                DO_XLSX_XLS=1
        fi
        if [[ "X$1" == "Xppt" || "X$1" == "Xall" ]]
        then
                PARS=$PARS" "ppt-pptx
                DO_PPT=1
        fi
        if [[ "X$1" == "Xppt-html" || "X$1" == "Xall" ]]
        then
                PARS=$PARS" "ppt-html
                DO_PPT_HTML=1
        fi
        if [[ "X$1" == "Xone" ]]
        then
                PARS=$PARS" "$1
                ONLY_ONE=1
        fi
        if [[ "X$1" == "Xlike" ]]
        then
                shift

                PARS=$PARS" ilike "$1
                LIKE=$1
        fi
        shift
done

if [[ -z "$PARS" ]]
then
        echo Must provide at least 1 valid parameter, none were provided
        echo useage:
        echo create-test OPTIONS
        echo where OPTIONS is at least one of:
        echo "There are two groups of options conversion tests and "
        echo "library processing tasks:"
        echo
        echo "   Conversion tasks which take file from the current"
        echo "   directory and process them putting the result into"
        echo "   the 'converted' subdirectory."
        echo
        echo "      html:  meaning run the tests of xlsx to html files."
        echo "      doc:   meaning run the tests of doc to docx files."
        echo "      ppt:   meaning run the tests of ppt to pptx files."
        echo "      xls:   meaning run the tests of xls to xlsx files."
        echo "      xls-html:   meaning run the tests of xls to html files."
        echo "      one:   means just run one test of any type specified"
        echo "             The test run is the one named alphabtically"
        echo "             as the first."
        echo "      like regexp the processed files must match the given"
        echo "                  regular expression regexp."
        echo
        echo "   Processing tasks:"
        echo "      clean: meaning extract the from the LibreOff*.tar.gz"
        echo "            built by the standard make."
        exit 1
fi

echo Active command line is : $PARS
rm -r -f converted
mkdir -p converted

if [ ! -f instdir/program/soffice.bin ]
then
        echo soffice.bin does not exist
        ./expand.sh
        echo expanded
fi

# takes 2 parameters the output type and the file
EXECUTE(){
        T=$(echo "$FILE" | grep "$LIKE")
        if [[ "X$T" != "X" ]]
        then
            echo processing $FILE
            instdir/program/soffice.bin \
                   --headless \
                   --norestore \
                   --invisible \
                   --nodefault \
                   --nofirststartwizard \
                   --nolockcheck \
                   --nologo \
                   --convert-to "$1" \
                   --outdir converted \
                    $2
        fi
}
if [[ $DO_HTML == 1 ]]
then
        echo testing xlsx and ods files
        for FILE in $(ls *.xlsx *.ods)
        do
                EXECUTE "html" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi

if [[ $DO_HTML_HIDE == 1 ]]
then
        echo testing xlsx and ods files in hide mode
        for FILE in $(ls *.xlsx *.ods)
        do
                EXECUTE "html:HTML (StarCalc):SkipLeadingBlanks" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi

if [[ $DO_DOC == 1 ]]
then
        echo testing doc to docx
        for FILE in $(ls *.doc)
        do
                EXECUTE "docx" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi

if [[ $DO_DOC_PDF == 1 ]]
then
        echo testing doc ior docx to pdf
        for FILE in $(ls *.doc *.docx)
        do
                EXECUTE "pdf" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi

if [[ $DO_PPT == 1 ]]
then
        echo testing ppt to pptx
        for FILE in $(ls *.ppt)
        do
                EXECUTE "pptx" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi

if [[ $DO_PPT_HTML == 1 ]]
then
        echo testing ppt/pptx to html
        for FILE in $(ls *.ppt *.pptx)
        do
                EXECUTE "html" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi

if [[ $DO_XLS == 1 ]]
then
        echo testing xls to xlsx
        for FILE in $(ls *.xls)
        do
                EXECUTE "xlsx" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi


if [[ $DO_XLS_HTML == 1 ]]
then
        echo testing xls to html
        for FILE in $(ls *.xls)
        do
                EXECUTE "html" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi
if [[ $DO_XLS_XLSX == 1 ]]
then
        echo testing xls to xlsx
        for FILE in $(ls *.xlsx)
        do
                EXECUTE "xls" "$FILE"
                if [[ $ONLY_ONE == 1 ]]
                then
                        break
                fi
        done
        echo
fi
# vim:set shiftwidth=4 softtabstop=4 expandtab:
