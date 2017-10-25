# UART_OUT
SI2_Project_For_PI_UART
2016/10/12

我們把arm 與 disarm 的功能搞錯，因此arm之後可以把螺旋槳啟動

2016/10/19

我們可以透過傳送arm command 來傳送 啟動指令，但是卻一直無法使用takeoff command !

2016/10/26

我們成功的藉由RC\_CHANNEL\_OVERRIDE 調整pixhawk 的 PPM 值，成功達成 上推throttle 的功能。目前遇到的問題是，在throttle固定之後，我們的pid並沒有在執行。

2016/10/29

        推測是不正確的mode導致pid沒有執行，測試不同的mode

        找到並trace成功案例發現是使用offboard mode

        試圖進入offboard mode失敗

2016/11/25

           我們需要去找offboard mode 的執行流程

2016/11/28

        測試網路上給的建議，將RC\_CHANNEL\_OVERRIDE寫入無窮迴圈中

        Pid仍然無法工作

2016/11/30

        測試RC\_CHANNEL\_OVERRIDE不同的參數設定

Pid仍然無法工作

2016/12/8

        在有GPS的環境下重新測試

Pid仍然無法工作

2016/12/14

        重新測試不同的參數與safety mode

Pid仍然無法工作，放棄RC\_CHANNEL\_OVERRIDE

2017/1/23

        延長開機後的系統等待時間

將current.sysid改為sysid

        測試STABLIZED\_ARM

        測試新command: [SET\_POSITION\_TARGET\_LOCAL\_NED](https://github.com/willtuna/UART_OUT/commit/9c7d18e34adda96db6e18599d791beb9770ea076)

        將sysid設為1

2017/1/25

        測試c\_uart\_example.cpp

        可以建立通訊

2017/2/3

重燒韌體後再測試將RC\_CHANNEL\_OVERRIDE寫入無窮迴圈中，Pid仍然無法工作

成功測試進入offboard mode

        測試y= -0.2，z= +0.5，在無GPS的情況下向北方暴衝

        測試不再將當前位置設置為下一個位置，改為設定固定位置，仍然暴衝

2017/2/4

        測試設為座標點0 10 0，這個點位於東方，仍然北衝

        測試先上升在下降，仍然北衝，但上下移動符合預期

        需要重新理解座標系統

2017/2/5

        重新測試新的座標系，並且設定座標 -1 0 1，仍然向北衝。

        提高傳送訊息頻率至4赫茲，沒有明顯改變。

2017/2/6

        改為測試LOCAL\_ALTITTUDE\_TARGET\_NED，沒有明顯的偏向，不過會飄移。

        測試自己設計的定高演算法，會上下震動甚至翻機。

2017/2/7

        改正定高演算法並重新測試，仍然失敗。

2017/2/8

同時測試LOCAL\_POSITION\_TARGET\_NED及LOCAL\_ALTITTUDE\_TARGET\_NED建構的定高演算法，皆失敗，沒有明顯進步

        印出Pixhawk所廣播的物理量，用以檢測飛控板是否按照操控邏輯走

2017/2/9

        繼續前一天測試，LOCAL\_POSITION\_TARGET\_NED成功，原因不明

        定高演算法失敗，狀況相同。

2017/2/10

        試圖重現前述成功，並找出原因。

        上下可以訂高，但在水平方位部分無法做到定位

        推測原因為座標系統問題

2017/2/13

        測試改變座標系統，行為模式不變，推測座標系統並沒有在韌體中按照document實做出來。

        重新推測為GPS訊號穩定程度

2017/2/16

        在GPS更為穩定的地方重新測試，測試LOCAL\_POSITION\_TARGET\_NED成功。

        測試LOCAL\_POSITION\_TARGET\_NED其他參數設定，沒有明顯效果，推測為尚未實作。

2017/2/17

        重現前述成功，並記錄成log檔。

2017/2/18

        測試此API，使無人機依照預設軌道飛行(方型)成功。

        同時再次測試不同坐標系，行為模式相同，確認座標系統在韌體中沒有實做出來。
