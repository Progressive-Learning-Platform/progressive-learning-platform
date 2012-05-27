/*
    Copyright 2012 PLP Contributors

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

package plptool.testsuite;

/**
 *
 * @author Wira
 */
public class StartupSanityCheck {
    public static void test_CallbackRegistry() {
        plptool.dmf.Callback testcall = new plptool.dmf.Callback() {
            private String msgStr;

            public boolean callback(int callbackNum, Object param) {
                System.out.println("callback #" + callbackNum + " called.");
                return false;
            }
        };
        int[] nums = {
                    plptool.dmf.CallbackRegistry.START,
                    plptool.dmf.CallbackRegistry.EXIT
                };
        plptool.dmf.CallbackRegistry.register(nums, testcall);
    }
}
