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

package plptool.dmf;

import plptool.*;
import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class CallbackRegistry {
    public static void setup() {

    }

    private static ArrayList<Callback> callbacks_Exit = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Start = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Command = new ArrayList<Callback>();

    private static ArrayList<Callback> callbacks_Project_New = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Project_Save = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Project_Open = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Project_Open_Successful = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Project_Open_Entry = new ArrayList<Callback>();

    private static ArrayList<Callback> callbacks_Load_Config_Line = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Save_Config = new ArrayList<Callback>();

    private static ArrayList<Callback> callbacks_Event_Simulate = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Desimulate = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Init = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Post_Init = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Post_Uninit = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Assemble = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Post_Assemble = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Program = new ArrayList<Callback>();

    private static ArrayList<Callback> callbacks_Event_Sim_Step_Aggregate = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Post_Step_Aggregate = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Run_Start = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Run_Stop = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Step = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Sim_Reset = new ArrayList<Callback>();

    private static ArrayList<Callback> callbacks_Event_Bus_Read = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Bus_Post_Read = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Bus_Write = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Bus_Eval = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Bus_Add = new ArrayList<Callback>();
    private static ArrayList<Callback> callbacks_Event_Bus_Remove = new ArrayList<Callback>();

    private static ArrayList<Callback> callbacks_Project_OpenAsm_Change = new ArrayList<Callback>();

    public static boolean callback_Exit()                                              {return performCallback(callbacks_Exit, null);}
    public static boolean callback_Start()                                             {return performCallback(callbacks_Start, null);}
    public static boolean callback_Command(String cmd)                                 {return performCallback(callbacks_Command, cmd);}

    public static boolean callback_Project_New()                                       {return performCallback(callbacks_Project_New, null);}
    public static boolean callback_Project_Save()                                      {return performCallback(callbacks_Project_Save, null);}
    public static boolean callback_Project_Open()                                      {return performCallback(callbacks_Project_Open, null);}
    public static boolean callback_Project_Open_Successful()                           {return performCallback(callbacks_Project_Open_Successful, null);}
    public static boolean callback_Project_Open_Entry(Object[] params)                 {return performCallback(callbacks_Project_Open_Entry, params);}

    public static boolean callback_Load_Config_Line(String[] config)                   {return performCallback(callbacks_Load_Config_Line, config);}
    public static boolean callback_Save_Config(java.io.FileWriter out)                 {return performCallback(callbacks_Save_Config, out);}

    public static boolean callback_Event_Simulate()                                    {return performCallback(callbacks_Event_Simulate, null);}
    public static boolean callback_Event_Desimulate()                                  {return performCallback(callbacks_Event_Desimulate, null);}
    public static boolean callback_Event_Sim_Init()                                    {return performCallback(callbacks_Event_Sim_Init, null);}
    public static boolean callback_Event_Sim_Post_Init()                               {return performCallback(callbacks_Event_Sim_Post_Init, null);}
    public static boolean callback_Event_Sim_Post_Uninit()                             {return performCallback(callbacks_Event_Sim_Post_Uninit, null);}
    public static boolean callback_Event_Assemble()                                    {return performCallback(callbacks_Event_Assemble, null);}
    public static boolean callback_Event_Post_Assemble()                               {return performCallback(callbacks_Event_Post_Assemble, null);}
    public static boolean callback_Event_Program()                                     {return performCallback(callbacks_Event_Program, null);}

    public static boolean callback_Event_Sim_Step_Aggregate(Integer steps)             {return performCallback(callbacks_Event_Sim_Step_Aggregate, steps);}
    public static boolean callback_Event_Sim_Post_Step_Aggregate()                     {return performCallback(callbacks_Event_Sim_Post_Step_Aggregate, null);}
    public static boolean callback_Event_Sim_Run_Start()                               {return performCallback(callbacks_Event_Sim_Run_Start, null);}
    public static boolean callback_Event_Sim_Run_Stop()                                {return performCallback(callbacks_Event_Sim_Run_Stop, null);}
    public static boolean callback_Event_Sim_Step()                                    {return performCallback(callbacks_Event_Sim_Step, null);}
    public static boolean callback_Event_Sim_Reset()                                   {return performCallback(callbacks_Event_Sim_Reset, null);}

    public static boolean callback_Event_Bus_Read(Long addr)                           {return performCallback(callbacks_Event_Bus_Read, addr);}
    public static boolean callback_Event_Bus_Post_Read(Object[] params)                {return performCallback(callbacks_Event_Bus_Post_Read, params);}
    public static boolean callback_Event_Bus_Write(Object[] params)                    {return performCallback(callbacks_Event_Bus_Write, params);}
    public static boolean callback_Event_Bus_Eval()                                    {return performCallback(callbacks_Event_Bus_Eval, null);}
    public static boolean callback_Event_Bus_Add(Object[] params)                      {return performCallback(callbacks_Event_Bus_Add, params);}
    public static boolean callback_Event_Bus_Remove(Object[] params)                   {return performCallback(callbacks_Event_Bus_Remove, params);}

    public static boolean callback_Project_OpenAsm_Change(Integer open_asm)            {return performCallback(callbacks_Project_OpenAsm_Change, open_asm);}

    private static boolean performCallback(ArrayList<Callback> callbacks, Object param) {
        boolean ret = false;
        for(int i = 0; i < callbacks.size(); i++) {
            Callback c = callbacks.get(i);
            Msg.D("callback: " + c.getClass().getCanonicalName(), 4, null);
            ret = c.callback(param) || ret;
        }
        return ret;
    }

    public static void register_Exit(Callback c)                                              {callbacks_Exit.add(c);}
    public static void register_Start(Callback c)                                             {callbacks_Start.add(c);}
    public static void register_Command(Callback c)                                           {callbacks_Command.add(c);}

    public static void register_Project_New(Callback c)                                       {callbacks_Project_New.add(c);}
    public static void register_Project_Save(Callback c)                                      {callbacks_Project_Save.add(c);}
    public static void register_Project_Open(Callback c)                                      {callbacks_Project_Open.add(c);}
    public static void register_Project_Open_Successful(Callback c)                           {callbacks_Project_Open_Successful.add(c);}
    public static void register_Project_Open_Entry(Callback c)                                {callbacks_Project_Open_Entry.add(c);}

    public static void register_Load_Config_Line(Callback c)                                  {callbacks_Load_Config_Line.add(c);}
    public static void register_Save_Config(Callback c)                                       {callbacks_Save_Config.add(c);}

    public static void register_Event_Simulate(Callback c)                                    {callbacks_Event_Simulate.add(c);}
    public static void register_Event_Desimulate(Callback c)                                  {callbacks_Event_Desimulate.add(c);}
    public static void register_Event_Sim_Init(Callback c)                                    {callbacks_Event_Sim_Init.add(c);}
    public static void register_Event_Sim_Post_Init(Callback c)                               {callbacks_Event_Sim_Post_Init.add(c);}
    public static void register_Event_Sim_Post_Uninit(Callback c)                             {callbacks_Event_Sim_Post_Uninit.add(c);}
    public static void register_Event_Assemble(Callback c)                                    {callbacks_Event_Assemble.add(c);}
    public static void register_Event_Post_Assemble(Callback c)                               {callbacks_Event_Post_Assemble.add(c);}
    public static void register_Event_Program(Callback c)                                     {callbacks_Event_Program.add(c);}

    public static void register_Event_Sim_Step_Aggregate(Callback c)                          {callbacks_Event_Sim_Step_Aggregate.add(c);}
    public static void register_Event_Sim_Post_Step_Aggregate(Callback c)                     {callbacks_Event_Sim_Post_Step_Aggregate.add(c);}
    public static void register_Event_Sim_Run_Start(Callback c)                               {callbacks_Event_Sim_Run_Start.add(c);}
    public static void register_Event_Sim_Run_Stop(Callback c)                                {callbacks_Event_Sim_Run_Stop.add(c);}
    public static void register_Event_Sim_Step(Callback c)                                    {callbacks_Event_Sim_Step.add(c);}
    public static void register_Event_Sim_Reset(Callback c)                                   {callbacks_Event_Sim_Reset.add(c);}

    public static void register_Event_Bus_Read(Callback c)                                    {callbacks_Event_Bus_Read.add(c);}
    public static void register_Event_Bus_Post_Read(Callback c)                               {callbacks_Event_Bus_Post_Read.add(c);}
    public static void register_Event_Bus_Write(Callback c)                                   {callbacks_Event_Bus_Write.add(c);}
    public static void register_Event_Bus_Eval(Callback c)                                    {callbacks_Event_Bus_Eval.add(c);}
    public static void register_Event_Bus_Add(Callback c)                                     {callbacks_Event_Bus_Add.add(c);}
    public static void register_Event_Bus_Remove(Callback c)                                  {callbacks_Event_Bus_Remove.add(c);}

    public static void register_Project_OpenAsm_Change(Callback c)                            {callbacks_Project_OpenAsm_Change.add(c);}
}
