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
        Callback_Test cb = new Callback_Test("app_events");
        int[] nums = {
                    plptool.dmf.CallbackRegistry.START,
                    plptool.dmf.CallbackRegistry.LOAD_CONFIG_LINE,
                    plptool.dmf.CallbackRegistry.SAVE_CONFIG,
                    plptool.dmf.CallbackRegistry.COMMAND,
                    plptool.dmf.CallbackRegistry.EVENT,
                    plptool.dmf.CallbackRegistry.EXIT
                };
        plptool.dmf.CallbackRegistry.register(nums, cb);
    }


    static class Callback_Test implements plptool.dmf.Callback {
        private String identifier;

        public Callback_Test(String identifier) {
            this.identifier = identifier;
        }

        public boolean callback(int callbackNum, Object param) {
            System.out.println("callback_test: " + identifier + " #" +callbackNum);
            return false;
        }
    }
}
