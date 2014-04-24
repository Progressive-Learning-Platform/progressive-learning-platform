/*
    Copyright 2014 PLP Contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

package plptool;

/**
 * This class represents an error that occurred during a build process of a
 * program (either with an assembler or compiler). The assembler abstract
 * maintains a list of this class' objects so a user interface can relate 
 * the errors back to the source files.
 *
 * @author Wira
 */
public class PLPBuildError {
    private int sourceFileIndex;
    private int sourceLineNumber;
    private String errorMessage;

    /**
     * The constructor for the class takes the index of the source file, the
     * line number of where the offending expression is in the file, and the
     * error message emitted by the build tool
     *
     * @param index Index of the source file
     * @param number Line number where the source of the error is
     * @param message Error message emitted by the build tool
     */
    public PLPBuildError(int index, int number, String message) {
        this.sourceFileIndex = index;
        this.sourceLineNumber = number;
        this.errorMessage = message;
    }

    /**
     * Get the index of the source file where the error originated
     *
     * @return Index of the source file
     */
    public int getSourceIndex() {
        return sourceFileIndex;
    }

    /**
     * Get the line number in the source file where the error originated
     *
     * @return Line number of the error originator
     */
    public int getLineNumber() {
        return sourceLineNumber;
    }

    /**
     * Get the error message emitted by the build tool
     *
     * @return Error message emitted by the build tool
     */
    public String getErrorMessage() {
        return errorMessage;
    }
}
