import sys, json
from datetime import datetime, timedelta

with open(sys.argv[1], 'r') as f:
    data = json.load(f)

name = data.get('name')
subjects_input = data.get('subjects', '')
exam_date = data.get('exam_date')
hours_per_day = int(data.get('hours_per_day', 2))

subjects = [s.strip() for s in subjects_input.split(',') if s.strip()]
today = datetime.now()
exam_day = datetime.strptime(exam_date, '%Y-%m-%d')
total_days = (exam_day - today).days

topic_map = {
    "DSA": ["Arrays", "Stacks", "Queues"],
    "DBMS": ["SQL", "Normalization"],
    "OS": ["Scheduling", "Memory Mgmt"],
    "CN": ["TCP/IP", "Routing"],
    "MATHS": ["Algebra", "Calculus"],
    "AI": ["ML", "NLP"]
}

plan = []
day_index = 0
while day_index < total_days:
    for subject in subjects:
        topics = topic_map.get(subject.upper(), [f"Topic {i+1}" for i in range(3)])
        for topic in topics:
            if day_index >= total_days:
                break
            date = today + timedelta(days=day_index)
            plan.append({
                "date": date.strftime('%Y-%m-%d'),
                "subject": subject,
                "topic": topic,
                "hours": hours_per_day
            })
            day_index += 1

with open(sys.argv[2], 'w') as f:
    json.dump({"plan": plan}, f)