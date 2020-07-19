using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Windows.ApplicationModel.AppService;
using Windows.Foundation.Collections;

namespace lxssex.WindowManagerAssistant
{
    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static async Task Main()
        {
            AppServiceConnection connection = new AppServiceConnection();
            connection.AppServiceName = "lxssex.wm.assistance";
            connection.PackageFamilyName = "lxssex.WindowManager_8vs4avx154rf0";
            connection.ServiceClosed += Connection_ServiceClosed;
            ValueSet set = new ValueSet
            {
                ["Request"] = "WaitInvoking"
            };
            await connection.SendMessageAsync(set);
        }

        private static void Connection_ServiceClosed(AppServiceConnection sender, AppServiceClosedEventArgs args)
        {
            throw new NotImplementedException();
        }
    }
}
