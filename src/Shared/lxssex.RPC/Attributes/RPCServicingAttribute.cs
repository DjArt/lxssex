using System;
using System.Collections.Generic;
using System.Text;

namespace lxssex.RPC.Attributes
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited = true)]
    public class RPCServicingAttribute : Attribute
    {
    }
}
