using lxssex.RPC;
using lxssex.WindowManager.Shared;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace lxssex.WindowManager.Systems.WindowManager
{
    public sealed class WindowHandler
    {
        private class ViewData
        {
            public int ID;
        }
        
        private readonly ConcurrentDictionary<CoreApplicationView, ViewData> Views = new ConcurrentDictionary<CoreApplicationView, ViewData>();
        private readonly Channel InputChannel;
        private readonly Channel OutputChannel;

        public WindowHandler()
        {

        }

        private async Task<int> CreateWindowInternal()
        {
            CoreApplicationView newView = CoreApplication.CreateNewView();
            int newViewId = -1;
            await newView.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                Frame frame = new Frame();
                frame.Navigate(typeof(WindowContent), null);
                Window.Current.Content = frame;
                Window.Current.Activate();

                newViewId = ApplicationView.GetForCurrentView().Id;
            });
            if (newViewId >= 0)
            {
                Views[newView] = new ViewData()
                {
                    ID = newViewId,
                };
                _ = await ApplicationViewSwitcher.TryShowAsViewModeAsync(newViewId, ApplicationViewMode.Default);
            }
            return newViewId;
        }

        private async Task CloseWindowInternal(int windowId)
        {
            if (windowId < 0) throw new ArgumentOutOfRangeException();

            CoreApplicationView view = Views.FirstOrDefault(x => x.Value.ID == windowId).Key;

            if (view is null) throw new ArgumentException();

            await view.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => view.CoreWindow.Close());
        }

        public WMError CreateWindow(out int windowId)
        {
            try
            {
                windowId = CreateWindowInternal().ConfigureAwait(true).GetAwaiter().GetResult();
                return WMError.Success;
            }
            catch (Exception ex)
            {
                windowId = -1;
                return WMError.InternalError;
            }
        }

        public WMError CloseWindow(int id)
        {
            try
            {
                CloseWindowInternal(id).ConfigureAwait(true).GetAwaiter().GetResult();
                return WMError.Success;
            }
            catch (ArgumentOutOfRangeException ex)
            {
                return WMError.ArgumentException;
            }
            catch (ArgumentException ex)
            {
                return WMError.WindowNotFound;
            }
            catch (Exception ex)
            {
                return WMError.InternalError;
            }
        }
    }
}
