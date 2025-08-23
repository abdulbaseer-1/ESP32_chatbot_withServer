import { useState, useEffect } from 'react';
import { Card, CardContent } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Select, SelectTrigger, SelectValue, SelectContent, SelectItem } from "@/components/ui/select";
import axios from 'axios';

export default function DeviceConfigDashboard() {
  const [config, setConfig] = useState({
    deviceId: '',
    username: '',
    password: '',
    voiceMode: 'male',
    aiMode: 'normal',
    location: '',
    wifiSsid: '',
    wifiPassword: ''
  });

  const [loading, setLoading] = useState(false);

  useEffect(() => {
    // Fetch config for a specific device ID
    const deviceId = 'esp_chatbot_001';
    axios.get(`/api/device-config/${deviceId}`)
      .then(response => setConfig(response.data))
      .catch(err => console.error(err));
  }, []);

  const handleChange = (e) => {
    const { name, value } = e.target;
    setConfig(prev => ({ ...prev, [name]: value }));
  };

  const handleSelectChange = (field, value) => {
    setConfig(prev => ({ ...prev, [field]: value }));
  };

  const handleSubmit = () => {
    setLoading(true);
    axios.post('/api/device-config/save', config)
      .then(() => alert('Configuration Saved Successfully'))
      .catch(err => alert('Failed to save configuration'))
      .finally(() => setLoading(false));
  };

  return (
    <div className="grid gap-6 p-6">
      <Card>
        <CardContent className="grid gap-4">
          <h2 className="text-xl font-bold">Device Configuration</h2>

          <Input name="deviceId" placeholder="Device ID" value={config.deviceId} onChange={handleChange} />
          <Input name="username" placeholder="Username" value={config.username} onChange={handleChange} />
          <Input name="password" type="password" placeholder="Password" value={config.password} onChange={handleChange} />

          <Select value={config.voiceMode} onValueChange={(value) => handleSelectChange('voiceMode', value)}>
            <SelectTrigger><SelectValue placeholder="Voice Mode" /></SelectTrigger>
            <SelectContent>
              <SelectItem value="male">Male</SelectItem>
              <SelectItem value="female">Female</SelectItem>
            </SelectContent>
          </Select>

          <Select value={config.aiMode} onValueChange={(value) => handleSelectChange('aiMode', value)}>
            <SelectTrigger><SelectValue placeholder="AI Mode" /></SelectTrigger>
            <SelectContent>
              <SelectItem value="normal">Normal</SelectItem>
              <SelectItem value="deep search">Deep Search</SelectItem>
            </SelectContent>
          </Select>

          <Input name="location" placeholder="Location" value={config.location} onChange={handleChange} />
          <Input name="wifiSsid" placeholder="WiFi SSID" value={config.wifiSsid} onChange={handleChange} />
          <Input name="wifiPassword" type="password" placeholder="WiFi Password" value={config.wifiPassword} onChange={handleChange} />

          <Button onClick={handleSubmit} disabled={loading}>{loading ? 'Saving...' : 'Save Configuration'}</Button>
        </CardContent>
      </Card>
    </div>
  );
}
