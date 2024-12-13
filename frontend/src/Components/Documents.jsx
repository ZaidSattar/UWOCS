const handleDownload = (filename) => {
    if (!ws) return;
    
    const handleDownloadMessage = (event) => {
        const data = JSON.parse(event.data);
        
        switch(data.type) {
            case 'downloadStart':
                console.log('Download starting:', data.filename);
                break;
                
            case 'downloadComplete':
                console.log('Download complete:', data.filename);
                ws.removeEventListener('message', handleDownloadMessage);
                break;
                
            case 'downloadError':
                console.error('Download error:', data.message);
                ws.removeEventListener('message', handleDownloadMessage);
                break;
                
            default:
                // Handle binary data (the actual file)
                if (event.data instanceof Blob) {
                    const blob = event.data;
                    const url = window.URL.createObjectURL(blob);
                    const a = document.createElement('a');
                    a.href = url;
                    a.download = filename;
                    document.body.appendChild(a);
                    a.click();
                    window.URL.revokeObjectURL(url);
                    document.body.removeChild(a);
                }
                break;
        }
    };

    // Add listener before sending request
    ws.addEventListener('message', handleDownloadMessage);
    
    // Send download request
    sendMessage({
        type: 'download',
        filename: filename
    });
}; 