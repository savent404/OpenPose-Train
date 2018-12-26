import QtQuick 2.0
import QtQuick.Controls 2.4
Item {
    id: element1
    property int value: 0
    property int value_top: 100
    property int value_bottom: 0
    width: 200
    height: 120
    anchors.horizontalCenter: parent.horizontalCenter
    Text {
        id: element
        text: qsTr("0")
        font.pointSize: 24
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
    }

    Button {
        id: up
        text: qsTr("+")
        autoRepeat: true
        height: 40
        anchors.top: parent.top
        anchors.right: parent.right
        onClicked: {
            if (value < parent.value_top) {
                value = value + 1
                element.text = parent.value.toString()
            }
        }
    }
    Button {
        id: down
        text: qsTr("-")
        autoRepeat: true
        height: 40
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        onClicked: {
            if (value > parent.value_bottom) {
                value = value - 1
                element.text = parent.value.toString()
            }
        }
    }
}
