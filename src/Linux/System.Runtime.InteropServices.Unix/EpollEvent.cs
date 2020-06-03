namespace System.Runtime.InteropServices.Unix
{
    internal struct EpollEvent
    {
        public EpollEventType Events;
        public EpollData Data;
    }
}
