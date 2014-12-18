/*
    Copyright 2013 PLP Contributors

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

import plptool.*;
import plptool.gui.*;
import plptool.testsuite.*;

import java.io.*;
import java.awt.*;

/**
 *
 * @author wira
 */
public class ToolboxTester implements Tester {
    private Robot r;

    public void configure(Robot r) {
        try {
            this.r = r;
        } catch(Exception e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }

    public void run(ProjectDriver plp) {    
        String tmp = null;
        String text;
        String text2;
        int ret;
        
        AutoTest.p("Testing PLPToolbox.getTmpDir method");
        try {
            tmp = PLPToolbox.getTmpDir();
        } catch(SecurityException e) {
            AutoTest.p("[E] Failed to create temporary directory");
            e.printStackTrace();
            System.exit(-1);
        }
        if(tmp == null) {
            AutoTest.p("[E] PLPToolbox.getTmpDir returned null");
            System.exit(-1);
        }
    
        AutoTest.p("Testing PLPToolbox.addToJar method");
        text = "Test with a newline\nCurrent time: " +
        System.currentTimeMillis() + "\n";
        ret = PLPToolbox.addToJar(tmp + "/autotest.1.jar", "test.txt",
                text.getBytes());
        if(ret != Constants.PLP_OK) {
            AutoTest.p("[E] FAILED");
            System.exit(-1);    
        }
        
        AutoTest.p("Testing PLPToolbox.copyFromJar method");
        File f = new File(tmp + "/test.txt");
        if(f.exists())
            f.delete();
        ret = PLPToolbox.copyFromJar(tmp + "/autotest.1.jar", "test.txt",
                                     tmp + "/test.txt");
        if(ret != Constants.PLP_OK) {
            AutoTest.p("[E] FAILED");
            System.exit(-1);    
        }
        
        AutoTest.p("Testing PLPToolbox.readFileAsString method");
        text2 = PLPToolbox.readFileAsString(tmp + "/test.txt");
        if(!text.equals(text2)) {
            AutoTest.p("[E] Text is different! FAILED");
            System.exit(-1);
        }   
    
        AutoTest.p("Adding a second file to the JAR file");
        AutoTest.p("- Writing");
        text = "Second file test with a newline\nCurrent time: " +
                System.currentTimeMillis() + "\n";
        ret = PLPToolbox.addToJar(tmp + "/autotest.1.jar", "test2.txt",
        text.getBytes());
        if(ret != Constants.PLP_OK) {
            AutoTest.p("[E] FAILED");
            System.exit(-1);    
        }
        
        AutoTest.p("- Extracting");
        f = new File(tmp + "/test2.txt");
        if(f.exists())
        f.delete();
        ret = PLPToolbox.copyFromJar(tmp + "/autotest.1.jar", "test2.txt",
                tmp + "/test2.txt");
        if(ret != Constants.PLP_OK) {
            AutoTest.p("[E] FAILED");
            System.exit(-1);    
        }
        
        AutoTest.p("- Comparing");
        text2 = PLPToolbox.readFileAsString(tmp + "/test2.txt");
        if(!text.equals(text2)) {
            AutoTest.p("[E] Text is different! FAILED");
            System.exit(-1);
        }   
            
        AutoTest.p("SUCCESS");
        System.exit(0);
    }
}

