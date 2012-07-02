import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.controlpanel 0.1
import MeeGo.Connman 0.2

Page {
    tools: DcpToolBar {}

    property NetworkService network
    property alias networkLabel: networkNameLabel.text

    Column {
        spacing: 10
        anchors { fill: parent }
        Item {
            anchors { left: parent.left; right: parent.right }
            height: 80

            Image {
                anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                source: "image://theme/icon-m-common-wlan-strength5"
                width: 60
                height: 60
            }

            Text {
                id: networkNameLabel
                anchors { left: parent.left; verticalCenter: parent.verticalCenter; leftMargin: 80 }
                text: "Network name"
                color: "white"
                font.pointSize: 18
            }
        }

        Item {
            anchors { left: parent.left; right: parent.right }
            height: 100
            Button {
                id: disconnectButton
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                text: "Disconnect"
                onClicked: {
                    console.log("Disconnect clicked");
                    network.requestDisconnect();
                    pageStack.pop();
                }
            }
        }
    }
}
