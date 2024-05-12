# Run with /system script run espkvm-push-power
# Useful: https://help.mikrotik.com/docs/display/ROS/Scripting

:local hostname "espkvm"
:local IP
:foreach i in=[/ip dhcp-server lease find where host-name="$hostname"] do={:set $IP ([/ip dhcp-server lease get $i address])}

:if ([:len $IP] = 0) do={:error "$hostname IP not found"}
:put "espkvm ip: $IP"

:local hz [/tool fetch url="http://$IP/hz" as-value output=user]
:if ($hz->"status" = "finished") do={
  :if ([:find ($hz->"data") "OK"] + 0 = 0) do={
    :local hzst ($hz->"data")
    :put "Health: $hzst"
    /tool fetch url="http://$IP/power" output=user http-method=post as-value
  } else={
    :error "Could not get health status"
  }
}
