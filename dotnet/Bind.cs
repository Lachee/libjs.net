using System.Runtime.InteropServices;

class Bind
{
    [DllImport("LibJSNet")]
    public static extern int run_script(string str);
}

