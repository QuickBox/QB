#!/bin/bash

[ "$EUID" -ne '0' ] && echo "Error,This script must be run as root! " && exit 1
[ $# -gt '1' ] && [ "$1" == '-f' ] && tmpKernelVer="$2" || tmpKernelVer='';
KernelVer='';
KernelBitVer='';
[ -z "$(dpkg -l |grep 'grub-')" ] && echo "Not found grub." && exit 1
MainURL='http://kernel.ubuntu.com/~kernel-ppa/mainline'
[ -n "$tmpKernelVer" ] && {
wget -qO /dev/null "$MainURL/$tmpKernelVer"
[ $? -ne '0' ] && echo 'Please input a vaild kernel version! exp: v4.11.9.' && exit 1
KernelVer="$tmpKernelVer"
}
[ -z "$tmpKernelVer" ] && {
KernelVerBIG="$(wget -qO- "$MainURL" |awk -F '/">|href="' '{print $2}' |sed '/rc/d;/^$/d' |tail -n1)"
[ -n "$KernelVerBIG" ] && KernelVer="$(wget -qO- "$MainURL" |awk -F '/">|href="' '{print $2}' |sed '/rc/d;/^$/d' |grep ''${KernelVerBIG}'' |sort -n |tail -n1)"
}
[ -z "$KernelVer" ] && echo 'Error,Get Kernel fail! ' && exit 1
ReleaseURL="$(echo -n "$MainURL/$KernelVer")"
KernelBit="$(getconf LONG_BIT)"
[ "$KernelBit" == '32' ] && KernelBitVer='i386'
[ "$KernelBit" == '64' ] && KernelBitVer='amd64'
[ -z "$KernelBitVer" ] && echo "Error! " && exit 1
KernelFile="$(wget -qO- "$ReleaseURL" |awk -F '">|href="' '/generic.*.deb/{print $2}' |grep 'image' |grep "$KernelBitVer" |head -n1)"
echo -ne "Download New Kernel\n\t$KernelFile\n"
wget -qO "$KernelFile" "$ReleaseURL/$KernelFile"
echo -ne "Install New Kernel\n\t$KernelFile\n"
dpkg -i "$KernelFile" >/dev/null 2>&1
Newest="$(echo "$KernelFile" |awk -F '_' '{print $1}')"
KernelList="$(dpkg -l |grep 'linux-image' |awk '{print $2}')"
[ -z "$(echo $KernelList |grep -o "$Newest")" ] && echo "Install error." && exit 1
for KernelTMP in `echo "$KernelList"`
 do
  [ "$KernelTMP" != "$Newest" ] && echo -ne "Uninstall Old Kernel\n\t$KernelTMP\n" && DEBIAN_FRONTEND=noninteractive dpkg --purge "$KernelTMP" >/dev/null 2>&1
done
[ "$(dpkg --get-selections |grep 'linux-image' |awk '{print $2}' |grep '^install' |wc -l)" != '1' ] && echo "Error, uninstall old Kernel." && exit 1
update-grub >/dev/null 2>&1
echo "pre-Loading TCP BBR ..."
[ ! -f /etc/sysctl.conf ] && touch /etc/sysctl.conf
sed -i '/net.core.default_qdisc.*/d' /etc/sysctl.conf
sed -i '/net.ipv4.tcp_congestion_control.*/d' /etc/sysctl.conf
echo "net.core.default_qdisc=fq" >>/etc/sysctl.conf
echo "net.ipv4.tcp_congestion_control=bbr" >>/etc/sysctl.conf
echo "Finish! "
