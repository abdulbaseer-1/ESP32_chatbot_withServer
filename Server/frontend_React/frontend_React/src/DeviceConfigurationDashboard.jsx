import { useState, useEffect } from 'react';
import axios from 'axios';

// Simple UI Components
const Card = ({ children }) => (
  <div style={{ border: '1px solid #ccc', padding: '1rem', borderRadius: '8px', marginBottom: '1rem' }}>
    {children}
  </div>
);

const CardContent = ({ children }) => (
  <div style={{ padding: '0.5rem 0' }}>
    {children}
  </div>
);

const Input = ({ id, name, value, onChange, placeholder, type = 'text' }) => (
  <input
    id={id}
    name={name}
    type={type}
    value={value}
    onChange={onChange}
    placeholder={placeholder}
    style={{ padding: '0.5rem', width: '100%', borderRadius: '4px', border: '1px solid #ccc', marginBottom: '1rem' }}
  />
);

const Select = ({ id, name, value, onChange, options }) => (
  <select
    id={id}
    name={name}
    value={value}
    onChange={onChange}
    style={{ padding: '0.5rem', width: '100%', borderRadius: '4px', border: '1px solid #ccc', marginBottom: '1rem' }}
  >
    {options.map((opt) => (
      <option key={opt.value} value={opt.value}>
        {opt.label}
      </option>
    ))}
  </select>
);

const Button = ({ children, onClick }) => (
  <button
    onClick={onClick}
    style={{ padding: '0.5rem 1rem', borderRadius: '8px', background: '#007bff', color: 'white', border: 'none' }}
  >
    {children}
  </button>
);

export default function DeviceConfigurationDashboard() {
  const [config, setConfig] = useState({
    deviceId: '',
    username: '',
    password: '',
    voiceMode: 'male',
    aiMode: 'normal',
    location: '',
    wifiSSID: '',
    wifiPassword: ''
  });

  const handleInputChange = (e) => {
    setConfig({ ...config, [e.target.name]: e.target.value });
  };

  const fetchConfig = async () => {
    try {
      const response = await axios.get(`/device-config/${config.deviceId}`);
      setConfig(response.data);
    } catch (error) {
      console.error('Error fetching config', error);
    }
  };

  const saveConfig = async () => {
    try {
      await axios.post('/device-config/save', config);
      alert('Configuration Saved!');
    } catch (error) {
      console.error('Error saving config', error);
    }
  };

  return (
    <div style={{ maxWidth: '600px', margin: '2rem auto', fontFamily: 'Arial, sans-serif' }}>
      <Card>
        <CardContent>
          <h2>Device Configuration</h2>

          <Input
            id="deviceId"
            name="deviceId"
            value={config.deviceId}
            onChange={handleInputChange}
            placeholder="Device ID"
          />

          <Button onClick={fetchConfig}>Fetch Config</Button>

          <Input
            id="username"
            name="username"
            value={config.username}
            onChange={handleInputChange}
            placeholder="Username"
          />

          <Input
            id="password"
            name="password"
            type="password"
            value={config.password}
            onChange={handleInputChange}
            placeholder="Password (Optional)"
          />

          <Select
            id="voiceMode"
            name="voiceMode"
            value={config.voiceMode}
            onChange={handleInputChange}
            options={[
              { value: 'male', label: 'Male' },
              { value: 'female', label: 'Female' }
            ]}
          />

          <Select
            id="aiMode"
            name="aiMode"
            value={config.aiMode}
            onChange={handleInputChange}
            options={[
              { value: 'normal', label: 'Normal' },
              { value: 'deep', label: 'Deep Search' }
            ]}
          />

          <Input
            id="location"
            name="location"
            value={config.location}
            onChange={handleInputChange}
            placeholder="Location"
          />

          <Input
            id="wifi_ssid"
            name="wifi_ssid"
            value={config.wifi_ssid}
            onChange={handleInputChange}
            placeholder="WiFi SSID"
          />

          <Input
            id="wifiPassword"
            name="wifiPassword"
            type="password"
            value={config.wifiPassword}
            onChange={handleInputChange}
            placeholder="WiFi Password"
          />

          <Button onClick={saveConfig}>Save Configuration</Button>
        </CardContent>
      </Card>
    </div>
  );
}
