/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool;

/**
 *
 * @author wira
 */
public class PLPToolbox {
    public static long parseNum(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Long.parseLong(number, 16);
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Long.parseLong(number, 2);
        }
        else
            return Long.parseLong(number);

        } catch(Exception e) {
            return PLPMsg.E("parse64Num(): Argument is not a valid number\n" + e,
                            PLPMsg.PLP_NUMBER_ERROR, null);
        }
    }

    // Convert 32-bit word to printable ASCII
    public static String asciiWord(long word) {
        String tStr = "";
        long tVal;
        for(int j = 3; j >= 0; j--) {
            tVal = word >> (8 * j);
            tVal &= 0xFF;
            if(tVal >= 0x21 && tVal <= 0x7E)
                tStr += (char) tVal + " ";
            else
                tStr += ". ";
        }

        return tStr;
    }
}
