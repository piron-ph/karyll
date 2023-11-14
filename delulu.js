const startDate = new Date('2023-11-14T00:00:00Z').getTime();

    function updateCounter() {
      const currentDate = new Date().getTime();
      const elapsedTime = currentDate - startDate;

      const seconds = Math.floor(elapsedTime / 1000) % 60;
      const minutes = Math.floor(elapsedTime / (1000 * 60)) % 60;
      const hours = Math.floor(elapsedTime / (1000 * 60 * 60)) % 24;
      const days = Math.floor(elapsedTime / (1000 * 60 * 60 * 24));

      const counterElement = document.getElementById('delulucounter');
      counterElement.textContent = `${days}d ${hours}h ${minutes}m ${seconds}s`;

      // Update the counter every second
      setTimeout(updateCounter, 1000);
    }

    // Initial call to start the counter
    updateCounter();