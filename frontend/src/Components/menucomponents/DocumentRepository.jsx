import React, { useState, useEffect } from 'react';
import { FaFilePdf, FaDownload, FaSearch, FaFolder } from 'react-icons/fa';
import '../styles/ComponentStyles.css';
import { useWebSocket } from '../../contexts/WebSocketContext.tsx';

const DocumentRepository = () => {
  const { ws, isConnected } = useWebSocket();
  const [searchTerm, setSearchTerm] = useState('');
  const [documents, setDocuments] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    if (ws && isConnected) {
      ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data);
          setDocuments(data);
          setLoading(false);
        } catch (err) {
          setError('Failed to parse server response');
          console.error('Parse error:', err);
        }
      };

      ws.onerror = (error) => {
        setError('WebSocket error');
        console.error('WebSocket error:', error);
      };

      ws.send(JSON.stringify({ type: 'get_files' }));
    }
  }, [ws, isConnected]);

  const handleDownload = async (filename) => {
    try {
      if (!ws) throw new Error('WebSocket not connected');
      
      ws.send(JSON.stringify({
        type: 'download',
        filename: filename
      }));

      // Handle binary response
      ws.onmessage = (event) => {
        const blob = new Blob([event.data], { type: 'application/pdf' });
        const url = window.URL.createObjectURL(blob);
        const link = document.createElement('a');
        link.href = url;
        link.download = filename;
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        window.URL.revokeObjectURL(url);
      };
    } catch (error) {
      console.error('Download failed:', error);
      alert('Failed to download file');
    }
  };

  // Filter documents based on search
  const filteredDocuments = documents.filter(doc =>
    doc.title.toLowerCase().includes(searchTerm.toLowerCase())
  );

  if (loading) return <div>Loading documents...</div>;
  if (error) return <div>Error: {error}</div>;

  return (
    <div className="document-container">
      <div className="document-header">
        <div className="header-content">
          <div className="header-left">
            <FaFolder className="header-icon" />
            <div className="header-text">
              <h1>Study Resources</h1>
              <p>Download course materials and study guides</p>
            </div>
          </div>
          <div className="search-bar">
            <FaSearch className="search-icon" />
            <input
              type="text"
              placeholder="Search documents..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
            />
          </div>
        </div>
      </div>

      <div className="document-grid">
        {filteredDocuments.map(doc => (
          <div key={doc.id} className="document-card">
            <div className="doc-icon-wrapper">
              <FaFilePdf className="doc-icon" />
            </div>
            <div className="doc-details">
              <h3>{doc.title}</h3>
              <p>PDF Document</p>
            </div>
            <button onClick={() => handleDownload(doc.filename)}>
              <FaDownload />
              <span> Download</span>
            </button>
          </div>
        ))}
      </div>
      
      {filteredDocuments.length === 0 && !loading && (
        <div className="no-documents">
          <FaFilePdf />
          <p>No documents found</p>
        </div>
      )}
    </div>
  );
};

export default DocumentRepository;