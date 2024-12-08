using System.Runtime.InteropServices;

class Bind
{
    [DllImport("LibJSExtern")]
    public static extern int run_script(string str);
}

