using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.System;

namespace lxssex.WindowManager.Systems
{
    public static class BackgroundSystem
    {
        private const string INVOKING_TASK = "lxssex Window Manager Invoking Task";
        private const string SERVICING_TASK = "lxssex Window Manager Servicing Task";
        private const string ASSISTANCE_TASK = "lxssex.wm.assistance";

        public async static Task ConfigureTasks(bool force = false)
        {
            await BackgroundExecutionManager.RequestAccessAsync();
            Dictionary<string, bool> taskRegistration = new Dictionary<string, bool>()
            {
                { INVOKING_TASK, false },
                { SERVICING_TASK, false },
            };
            foreach (IBackgroundTaskRegistration registeredTask in BackgroundTaskRegistration.AllTasks.Select(x => x.Value))
            {
                switch (registeredTask.Name)
                {
                    case string taskName when taskRegistration.Keys.Contains(taskName):
                        if (force)
                        {
                            goto default;
                        }
                        else
                        {
                            taskRegistration[taskName] = true;
                        }
                        break;
                    default:
                        registeredTask.Unregister(false);
                        break;
                }
            }
            foreach (string taskName in taskRegistration.Keys)
            {
                if (!taskRegistration[taskName])
                {
                    BackgroundTaskBuilder taskBuilder = new BackgroundTaskBuilder();
                    taskBuilder.Name = taskName;
                    switch (taskName)
                    {
                        case INVOKING_TASK:
                            taskBuilder.SetTrigger(new CustomSystemEventTrigger("GUID", CustomSystemEventTriggerRecurrence.Always));
                            break;
                        case SERVICING_TASK:
                            taskBuilder.SetTrigger(new SystemTrigger(SystemTriggerType.ServicingComplete, false));
                            break;
                        default:
                            throw new NotImplementedException($"Case for {taskName} task missed.");
                    }
                    taskBuilder.Register();
                }
            }
        }

        public async static Task ProcessBackgroundTask(IBackgroundTaskInstance taskInstance, BackgroundTaskDeferral deferral)
        {
            switch(taskInstance.Task.Name)
            {
                case INVOKING_TASK:

                    break;
                case ASSISTANCE_TASK:

                    break;
                default:
                    throw new NotImplementedException($"Case for {taskInstance.Task.Name} task missed.");
            }
        }
    }
}
