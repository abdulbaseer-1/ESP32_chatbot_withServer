export const Card = ({ children }) => (
  <div style={{ border: '1px solid #ccc', padding: '1rem', borderRadius: '8px', marginBottom: '1rem' }}>
    {children}
  </div>
);

export const CardContent = ({ children }) => (
  <div style={{ padding: '0.5rem 0' }}>
    {children}
  </div>
);
