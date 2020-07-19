using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.AppService;
using Windows.ApplicationModel.Background;
using Windows.Foundation.Collections;

namespace lxssex.WindowManager.Systems.Background
{
    public sealed class AssistanceTask : IDisposable
    {
        private class RequestData
        {
            public AppServiceDeferral Deferral { get; set; }
            public AppServiceRequest Request { get; set; }
            public Task WaitConditions { get; set; }
            public ManualResetEventSlim ConditionsSuccess { get; set; }
            public CancellationTokenSource TokenSource { get; set; }
        }

        public const string WAIT_INVOKING_REQUEST = "WaitInvoking";

        private static void WaitConditions(RequestData request)
        {
            request.ConditionsSuccess.Wait(request.TokenSource.Token);
        }

        private bool disposedValue;
        private readonly AppServiceConnection Connection;
        private readonly BackgroundTaskDeferral Deferral;
        private readonly ConcurrentDictionary<string, ConcurrentQueue<RequestData>> Requests;

        public AssistanceTask(AppServiceConnection connection, BackgroundTaskDeferral deferral)
        {
            Requests = new ConcurrentDictionary<string, ConcurrentQueue<RequestData>>();
            Requests[WAIT_INVOKING_REQUEST] = new ConcurrentQueue<RequestData>();
            Connection = connection;
            Deferral = deferral;
            Connection.RequestReceived += OnRequestReceived;
            Connection.ServiceClosed += OnServiceClosed;
        }

        private void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    foreach (ConcurrentQueue<RequestData> queue in Requests.Values)
                    {
                        while (queue.TryDequeue(out RequestData request))
                        {
                            request.TokenSource.Cancel();
                            request.ConditionsSuccess.Dispose();
                            request.Deferral.Complete();
                            request.WaitConditions.Wait();
                            request.WaitConditions.Dispose();
                        }
                    }
                    Deferral.Complete();
                }
                disposedValue = true;
            }
        }

        private void OnServiceClosed(AppServiceConnection sender, AppServiceClosedEventArgs args)
        {
            Dispose();
        }

        private void OnRequestReceived(AppServiceConnection sender, AppServiceRequestReceivedEventArgs args)
        {
            AppServiceDeferral deferral = args.GetDeferral();
            if (args.Request.Message.TryGetValue("Request", out object _request) && _request is string request)
            {
                switch (request)
                {
                    case WAIT_INVOKING_REQUEST:
                        RequestData data = new RequestData()
                        {
                            ConditionsSuccess = new ManualResetEventSlim(),
                            Deferral = deferral,
                            Request = args.Request,
                            TokenSource = new CancellationTokenSource()
                        };
                        data.WaitConditions = Task.Run(() => WaitConditions(data));
                        Requests[request].Enqueue(data);
                        break;
                    default:
                        //Not supported request
                        deferral.Complete();
                        break;
                }
            }
        }

        public void Dispose()
        {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        public async Task OnEventOccurred(string eventType, ValueSet eventData)
        {
            switch (eventType)
            {
                case WAIT_INVOKING_REQUEST:
                    if (Requests[eventType].TryDequeue(out RequestData request))
                    {
                        request.ConditionsSuccess.Set();
                        await request.Request.SendResponseAsync(eventData);
                        request.Deferral.Complete();
                    }
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(eventType));
            }
        }

        public void OnTaskCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            Dispose();
        }
    }
}
