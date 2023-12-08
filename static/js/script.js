document.addEventListener('DOMContentLoaded', function() {
    function updateStatus() {
        fetch('/get_status')
            .then(response => response.json())
            .then(data => {
                // Update status for sensor1
                const statusElement1 = document.getElementById('status1');
                const parkingSpotImage1 = document.getElementById('parkingSpotImage1'); // Get the image element

                if (data.sensor1) {
                    statusElement1.textContent = "Occupied";
                    statusElement1.className = "text-danger";
                    parkingSpotImage1.src = "/static/img/occupied.jpg"; // Update image for occupied
                } else {
                    statusElement1.textContent = "Empty";
                    statusElement1.className = "text-success";
                    parkingSpotImage1.src = "/static/img/free.jpg"; // Update image for empty
                }

                // Update status for sensor2
                const statusElement2 = document.getElementById('status2');
                const parkingSpotImage2 = document.getElementById('parkingSpotImage2'); // Corrected variable name

                if (data.sensor2) {
                    statusElement2.textContent = "Occupied";
                    statusElement2.className = "text-danger";
                    parkingSpotImage2.src = "/static/img/occupied.jpg"; // Corrected image update
                } else {
                    statusElement2.textContent = "Empty";
                    statusElement2.className = "text-success";
                    parkingSpotImage2.src = "/static/img/free.jpg"; // Update image for empty
                }
            })
            .catch(error => console.error('Error fetching status:', error));
    }

    function updateUtils() {
        fetch('/get_utils')
            .then(response => response.json())
            .then(data => {
                // Update the elements on your page that display battery and signal strength
                const batteryLevelElements = document.querySelectorAll('.batteryLevel');
                const wifiSignalImages = document.querySelectorAll('.wifiSignal');

                batteryLevelElements.forEach(element => {
                    element.textContent = data.battery + '%';
                });


                // Determine the image filename based on signal strength
                let signalImg = 'low-signal.png'; // Default image
                if (data.signalStrength === 2) {
                    signalImg = 'mid-signal.png';
                } else if (data.signalStrength === 3) {
                    signalImg = 'high-signal.png';
                } else if (data.signalStrength === 4) {
                    signalImg = 'full-signal.png';
                }

                // Update each WiFi signal image
                wifiSignalImages.forEach(img => {
                img.src = "/static/img/" + signalImg + "";
                });

            })
            .catch(error => console.error('Error fetching utils:', error));
    }
    
    // Update the status every 10 seconds
    setInterval(updateStatus, 10000);
    setInterval(updateUtils, 10000); 
    updateStatus();
    updateUtils();

});

document.addEventListener('DOMContentLoaded', function() {
    function updateStatus() {

        updateDate(); // Call the updateDate function within updateStatus
    }

    function updateDate() {
        const dateElements = document.querySelectorAll('.blog-slider__code'); // Select the date element
        const currentDate = new Date();
        const formattedDate = currentDate.toLocaleDateString('en-US', { 
            day: 'numeric', 
            month: 'long', 
            year: 'numeric' 
        });

        dateElements.forEach(function(element) {
        element.textContent = formattedDate;
    });
    }
    updateStatus(); // Initial call to update status and date
});
