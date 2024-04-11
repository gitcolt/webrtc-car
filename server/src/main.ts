import './style.css'

document.querySelector<HTMLDivElement>('#app')!.innerHTML = `
  <div>
    <select id="availableCameras"></select>
  </div>
`

async function openMediaDevices(constraints: MediaStreamConstraints) {
    return await navigator.mediaDevices.getUserMedia(constraints);
}

try {
    const stream = await openMediaDevices({ 'video': true, 'audio': true });
    console.log('Got MediaStream: ', stream);
} catch (error) {
    console.log(`Error accessing media devices: ${error}`);
}

function updateCameraList(cameras: MediaDeviceInfo[]) {
    const listEl: HTMLSelectElement = document.querySelector('select#availableCameras')!;
    listEl.innerHTML = '';
    cameras.forEach(camera => {
        const cameraOption = document.createElement('option');
        cameraOption.label = camera.label;
        cameraOption.value = camera.deviceId;
        listEl.add(cameraOption);
    });
}

async function getConnectedDevices(type: MediaDeviceKind) {
    const devices = await navigator.mediaDevices.enumerateDevices();
    return devices.filter(dev => dev.kind == type);
}

const videoCameras = await getConnectedDevices('videoinput');
updateCameraList(videoCameras);

navigator.mediaDevices.addEventListener('devicechange', async _ => {
    const newCameraList = await getConnectedDevices('videoinput');
    updateCameraList(newCameraList);
});
