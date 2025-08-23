import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import DeviceConfigDashboard from './DeviceConfigurationDashboard'

createRoot(document.getElementById('root')).render(
  <StrictMode>
    <DeviceConfigDashboard />
  </StrictMode>,
)
