#!/bin/bash
echo -e "\nRebuilding the User Manual"
echo -e "\nCreating new PDF..."
pdflatex UserManual.tex > rebuild.log
echo -e "\nDone creating PDF, moving on to multi-page HTML..."
latex2html UserManual.tex -show_section_numbers >> rebuild.log
echo -e "\nDone creating multi-page HTML, moving on to single-page HTML..."
latex2html UserManual.tex -show_section_numbers -split 0 -dir UserManualSingle >> rebuild.log
echo -e "\nDone rebuilding the User Manual. Please see rebuild.log for additional details."
