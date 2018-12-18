import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtMultimedia 5.9
import MyProb 1.0

Window {
    id: main_window
    visible: true
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    color: "grey"
    title: qsTr("OpenPose Train")

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: page_main

        Component {
            id: page_main
            Item {
                id: page_main_item
                anchors.fill:parent
                ColumnLayout {
                    anchors.fill: parent
                    Button {
                        text: qsTr("仰卧角度保持")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onClicked: stackView.push(page_pose1)
                    }
                    Button {
                        text: qsTr("动态臀桥")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onClicked: stackView.push(page_pose2)
                    }
                    Button {
                        text: qsTr("自定义")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onClicked: stackView.push(page_poseUnknow)
                    }
                }
            }
        }

        Component {
            id: page_pose1
            Item {
                id: page_pose1_item
                anchors.fill: parent
                BackButton {
                    onClicked: stackView.pop()
                }
                StartButton {
                    onClicked: stackView.push(page_camera)
                }
                PoseInfo {
                    text:qsTr("<pre><h4>步骤</h4><ul><li>仰卧在垫面上，双手放在臀部两侧，掌心向下压实垫面</li><li>吸气屈双膝，双腿向上伸直</li><li>呼气，脚趾回勾，控制住双腿保持在垂直于垫面的位置</li><li>控制住双腿保持在垂直于垫面的位置</li><li>吸气蹬直双膝</li><li>呼气尝试将双腿水平落下到于垫面60度的位置</li></ul><h4>呼吸</h4><ul><li>保持均匀的呼吸</li></ul><h4>动作感受</h4><ul><li>腹部收紧，大腿后侧有很强拉伸感</li></ul><h4>常见问题</h4><ul><li>问题：腰部不要腾空</li><li>解决：腹部下方垫一块小毛毯</li></ul></pre>")

                }

                Image {
                    id: page_pose1_image
                    anchors.fill: parent
                    source: "src_img/train_1.jpg"
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
        Component {
            id: page_pose2
            Item {
                id: page_pose2_item
                anchors.fill: parent
                BackButton {
                    onClicked: stackView.pop()
                }
                StartButton {
                    onClicked: stackView.push(page_camera)
                }
                PoseInfo {
                    text: qsTr("<pre><h4>步骤</h4><ul><li>平躺于垫面上，屈双膝，双脚掌踩实垫面</li><li>双脚分开与髋同宽，小腿垂直于垫面，双手放在臀部两侧</li><li>掌心向下，手肘靠近身体</li><li>吸气，抬高大腿臀部和下背部</li><li>呼气，缓缓将脊柱一节一节下落，落下臀部</li></ul><h4>呼吸</h4><ul><li>跟随动作起伏进行深长的呼吸</li></ul><h4>动作感受</h4><ul><li>臀部和腹部收紧，大腿发力</li></ul><h4>常见问题</h4><ul><li>问题：腰部疼痛</li><li>解决：收紧臀腹</li></ul></pre>")
                }

                Image {
                    id: page_pose2_image
                    anchors.fill: parent
                    source: "src_img/train_2.jpg"
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
        Component {
            id: page_poseUnknow
            Item {
                id: page_poseUnknow_item
                anchors.fill: parent
                BackButton {
                    onClicked: stackView.pop()
                }
                StartButton {
                    onClicked: stackView.push(page_camera)
                }
            }
        }
        Component {
            id: page_camera
            Item {
                id: page_camera_item
                width: 640
                height: 360
                anchors.top: parent.top
                anchors.topMargin: 30

                BackButton {
                    onClicked: stackView.pop()
                }
                Camera {
                    id:camera
                    imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash

                    exposure {
                        exposureCompensation: -1.0
                        exposureMode: Camera.ExposurePortrait
                    }

                    flash.mode: Camera.FlashRedEyeReduction
                }
                MyProb {
                    id: prob
                    qmlCamera: camera
                    timeout: false
                }
                VideoOutput {
                    source: camera
                    anchors.fill: parent
                    focus: visible
                }
                Timer {
                    interval: 500
                    running: true
                    repeat: true
                    onTriggered: prob.timeout = true
                }
            }
        }
    }
}
